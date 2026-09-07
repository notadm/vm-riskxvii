
```text
===========================================================================

                            vm_riskxvii
               a Pseudo-RISC-V (RV32I subset) Virtual Machine

    Author  : Adam Zhao <im@notadam.com>
    Compile : make vm
    Tests   : make run_tests

---------------------------------------------------------------------------

  ARCHITECTURE

    32 x 32-bit general purpose registers  (R[0] hardwired to zero)
    PC initialised to zero on startup
    2048-byte memory image loaded from binary file on launch

    Memory Map

      0x0000 - 0x03FF   instruction memory        (1024 bytes)
      0x0400 - 0x07FF   data / stack memory        (1024 bytes)
      0x0800 - 0x08FF   virtual routines
      0xB700 - 0xD6FF   heap banks          (128 x 64 bytes = 8192)

---------------------------------------------------------------------------

  ENCODING

    each instruction is 4 bytes wide. six encoding formats are used:

    +--------------+----------+----------+------+-------------+--------------+
    |  31      25  |  24   20 |  19   15 |14  12|  11      7  |   6       0  |
    +--------------+----------+----------+------+-------------+--------------+
    |  func7       |   rs2    |   rs1    | fn3  |     rd      |    opcode    |  R
    |        imm[11:0]        |   rs1    | fn3  |     rd      |    opcode    |  I
    |  imm[11:5]   |   rs2    |   rs1    | fn3  |  imm[4:0]   |    opcode    |  S
    | imm[12|10:5] |   rs2    |   rs1    | fn3  | imm[4:1|11] |    opcode    |  SB
    |                imm[31:12]                 |     rd      |    opcode    |  U
    |           imm[20|10:1|11|19:12]           |     rd      |    opcode    |  UJ
    +--------------+----------+----------+------+-------------+--------------+

    opcode    identifies the instruction and encoding type
    rd        destination register (5 bits, 32 regs)
    rs1 rs2   source registers     (5 bits each)
    func3     sub-operation selector (3 bits)
    func7     sub-operation selector (7 bits, R-type only)
    imm       immediate value, bits may be scrambled across the word

    program layout in memory

      [ instr 0 : 32 bits ]
      [ instr 1 : 32 bits ]
      [ instr 2 : 32 bits ]
      [ ...               ]
      [ instr n : 32 bits ]

---------------------------------------------------------------------------

  INSTRUCTION SET

    #   mnemonic  type   opcode     fn3   fn7        operation
    --  --------  ----   -------    ---   -------    ------------------------------------------
    Arithmetic
     1  add       R      0110011    000   0000000    R[rd] = R[rs1] + R[rs2]
     2  sub       R      0110011    000   0100000    R[rd] = R[rs1] - R[rs2]
     3  addi      I      0010011    000              R[rd] = R[rs1] + imm
     4  lui       U      0110111                     R[rd] = { imm[31:12], 12'b0 }

    Logical
     5  xor       R      0110011    100   0000000    R[rd] = R[rs1] ^ R[rs2]
     6  xori      I      0010011    100              R[rd] = R[rs1] ^ imm
     7  or        R      0110011    110   0000000    R[rd] = R[rs1] | R[rs2]
     8  ori       I      0010011    110              R[rd] = R[rs1] | imm
     9  and       R      0110011    111   0000000    R[rd] = R[rs1] & R[rs2]
    10  andi      I      0010011    111              R[rd] = R[rs1] & imm

    Shift
    11  sll       R      0110011    001   0000000    R[rd] = R[rs1] << R[rs2]
    12  srl       R      0110011    101   0000000    R[rd] = R[rs1] >> R[rs2]
    13  sra       R      0110011    101   0100000    R[rd] = rotr(R[rs1], R[rs2])

    Load
    14  lb        I      0000011    000              R[rd] = sext( M[R[rs1]+imm][ 7:0] )
    15  lh        I      0000011    001              R[rd] = sext( M[R[rs1]+imm][15:0] )
    16  lw        I      0000011    010              R[rd] = M[R[rs1]+imm]
    17  lbu       I      0000011    100              R[rd] = M[R[rs1]+imm][ 7:0]  (zero-ext)
    18  lhu       I      0000011    101              R[rd] = M[R[rs1]+imm][15:0]  (zero-ext)

    Store
    19  sb        S      0100011    000              M[R[rs1]+imm] = R[rs2][ 7:0]
    20  sh        S      0100011    001              M[R[rs1]+imm] = R[rs2][15:0]
    21  sw        S      0100011    010              M[R[rs1]+imm] = R[rs2]

    Compare / Set
    22  slt       R      0110011    010   0000000    R[rd] = (R[rs1]  < R[rs2])  ? 1 : 0
    23  slti      I      0010011    010              R[rd] = (R[rs1]  < imm)     ? 1 : 0
    24  sltu      R      0110011    011   0000000    R[rd] = (R[rs1]  < R[rs2])  ? 1 : 0  (u)
    25  sltiu     I      0010011    011              R[rd] = (R[rs1]  < imm)     ? 1 : 0  (u)

    Branch
    26  beq       SB     1100011    000              if (R[rs1] == R[rs2]) PC = PC + (imm<<1)
    27  bne       SB     1100011    001              if (R[rs1] != R[rs2]) PC = PC + (imm<<1)
    28  blt       SB     1100011    100              if (R[rs1]  < R[rs2]) PC = PC + (imm<<1)
    29  bltu      SB     1100011    110              if (R[rs1]  < R[rs2]) PC = PC + (imm<<1) (u)
    30  bge       SB     1100011    101              if (R[rs1] >= R[rs2]) PC = PC + (imm<<1)
    31  bgeu      SB     1100011    111              if (R[rs1] >= R[rs2]) PC = PC + (imm<<1) (u)

    Jump
    32  jal       UJ     1101111                     R[rd] = PC+4 ; PC = PC + (imm<<1)
    33  jalr      I      1100111    000              R[rd] = PC+4 ; PC = R[rs1] + imm

---------------------------------------------------------------------------

  VIRTUAL ROUTINES

    Virtual routines are operations mapped to specific memory addresses such
    that a memory access at that address will have different effects.  This
    allows programs running in the virtual machine to communicate with the
    outside world through input/output (I/O) operations.

    1  0x0800 - Console Write Character
       A memory store command to this address will cause the virtual machine
       to print the value being stored as a single ASCII encoded character
       to stdout.

    2  0x0804 - Console Write Signed Integer
       A memory store command to this address will cause the virtual machine
       to print the value being stored as a single 32-bit signed integer in
       decimal format to stdout.

    3  0x0808 - Console Write Unsigned Integer
       A memory store command to this address will cause the virtual machine
       to print the value being stored as a single 32-bit unsigned integer in
       lower case hexadecimal format to stdout.

    4  0x080C - Halt
       A memory store command to this address will cause the virtual machine
       to halt the current running program, then output "CPU Halt Requested"
       to stdout, and exit, regardless of the value being stored.

    5  0x0812 - Console Read Character
       A memory load command to this address will cause the virtual machine
       to scan input from stdin and treat the input as an ASCII-encoded
       character for the memory load result.

    6  0x0816 - Console Read Signed Integer
       A memory load command to this address will cause the virtual machine
       to scan input from stdin and parse the input as a signed integer for
       the memory load result.

    7  0x0820 - Dump PC
       A memory store command to this address will cause the virtual machine
       to print the value of PC in lower case hexadecimal format to stdout.

    8  0x0824 - Dump Register Banks
       A memory store command to this address will force the virtual machine
       to perform a Register Dump.  See Error Handling.

    9  0x0828 - Dump Memory Word
       A memory store command to this address will cause the virtual machine
       to print the value of M[v] in lower case hexadecimal format to stdout.
       v is the value being stored interpreted as a 32-bit unsigned integer.

    10  0x0830, 0x0834 - Heap Banks
        These addresses are used by a hardware-enabled memory allocation
        system.  See Heap Banks.

    11  0x0850 and above - Reserved
        These addresses will not be called by test programs.

  ERROR HANDLING

    Register Dump
      Prints the value of all registers including PC in lower case hex,
      one register per line:

        PC = 0x00000001;
        R[0] = 0xffffffff;
        R[1] = 0xffffffff;
        ...
        R[31] = 0xffffffff;

    Not Implemented
      If an unknown instruction is detected, output to stdout then Register
      Dump and terminate:

        Instruction Not Implemented: 0xffffffff

    Illegal Operation
      When an illegal operation is raised (e.g. access to unallocated heap),
      output to stdout then Register Dump and terminate:

        Illegal Operation: 0x<encoded instruction>

---------------------------------------------------------------------------

  HEAP

    Two independent 4096-byte regions, each divided into 64-byte banks.
    Allocation is bitmap-managed; size is rounded up to the next power of 2.

    layout
      region    address range          banks
      -------   --------------------   -----
      LOWER     0xB700 .. 0xBEFF       64 banks x 64 B = 4096 B
      UPPER     0xC700 .. 0xD6FF       64 banks x 64 B = 4096 B
      total                            128 banks        = 8192 B

    allocation rules
      - minimum allocation : 64 bytes  (1 bank)
      - maximum allocation : 8192 bytes (entire heap, both regions)
      - size is rounded up to next power of 2 before searching
      - LOWER region is tried first; UPPER is used if LOWER has no space
      - result pointer is written to R[28] (vmalloc returns 0 on failure)

    size classes
      request range    allocated size   banks used
      -------------    --------------   ----------
         1 –  64 B         64 B            1
        65 – 128 B        128 B            2
       129 – 256 B        256 B            4
       257 – 512 B        512 B            8
       513 – 1024 B      1024 B           16
      1025 – 2048 B      2048 B           32
      2049 – 4096 B      4096 B           64
      4097 – 8192 B      8192 B          128  (both regions must be free)

    bitmap encoding
      VM.allocation         64-bit word per region; 1 bit = 1 bank in use
      VM.allocsize[0..3]    4-bit per-bank size tag (log2 of blocks used)

===========================================================================
```

```text
===========================================================================

  EXAMPLE 1 - Printing "H"

  Prints the first letter of "Hello, World" to stdout.

  C equivalent:

    char volatile *const ConsoleWriteChar = (char *)0x0800;

    int main() {
        *ConsoleWriteChar = 'H';
        return 0;
    }

  RISK-XVII assembly:

    00000000 <_start>:
       0:   7ff00113        addi  sp, x0, 2047
       4:   00c000ef        jal   ra, 10 <main>
       8:   000017b7        lui   a5, 0x1
       c:   80078623        sb    zero, -2036(a5)

    00000010 <main>:
      10:   000017b7        lui   a5, 0x1
      14:   04800713        addi  a4, x0, 72
      18:   80e78023        sb    a4, -2048(a5)
      1c:   00000513        addi  a0, x0, 0
      20:   00008067        ret

  Notes:
    - <_start> initialises the stack pointer and jumps to main.
    - ret is jalr with rs1 = 1 (ra).
    - sb to 0x0800 triggers VR_WR_CHAR -> prints 'H'.
    - sb to 0x080C (via _start halt stub) triggers VR_HALT.

  Memory image (9 x 4-byte words then padding):

    [1301F07F]   <- addi sp, x0, 2047  (little-endian)
    [EF00C000]   <- jal  ra, 10
    [B7170000]   <- lui  a5, 0x1
    [23860780]   <- sb   zero, -2036(a5)
    [B7170000]   <- lui  a5, 0x1
    [13078004]   <- addi a4, x0, 72
    [2380E780]   <- sb   a4, -2048(a5)
    [13050000]   <- addi a0, x0, 0
    [67800000]   <- ret
    [988 bytes of padding for instruction memory]
    [1024 bytes of padding for data memory]

  The input memory image binary file is always 2048 bytes.

===========================================================================
```

```text
===========================================================================

  EXAMPLE 2 - Adding Two Numbers

  Scans two signed integers from stdin, prints their sum to stdout.

  C equivalent:

    int volatile *const ConsoleWriteSInt = (int *)0x0804;

    inline int scan_char() {
        int result;
        int addr = 0x0816;
        asm volatile("lw %[res], 0(%[adr])"
                     : [res]"=r"(result)
                     : [adr]"r"(addr));
        return result;
    }

    int main() {
        int a = scan_char();
        int b = scan_char();
        *ConsoleWriteSInt = a + b;
        return 0;
    }

  RISK-XVII assembly:

    00000000 <_start>:
       0:   7ff00113        li    sp, 2047
       4:   00c000ef        jal   ra, 10 <main>
       8:   000017b7        lui   a5, 0x1
       c:   80078623        sb    zero, -2036(a5)

    00000010 <main>:
      10:   00001737        lui   a4, 0x1
      14:   81670793        addi  a5, a4, -2026
      18:   0007a683        lw    a3, 0(a5)
      1c:   0007a783        lw    a5, 0(a5)
      20:   00d787b3        add   a5, a5, a3
      24:   80f72223        sw    a5, -2044(a4)
      28:   00000513        li    a0, 0
      2c:   00008067        ret

===========================================================================
```

```text
===========================================================================

  EXAMPLE 3 - 5 Sum

  Scans up to five non-zero integers from stdin and prints their sum.
  Stops early if a zero is read.  Uses scan_char() from Example 2.

  C equivalent:

    int volatile *const ConsoleWriteSInt = (int *)0x0804;

    int main() {
        int count = 0;
        int sum   = 0;
        int num;

        while (count++ < 5) {
            num = scan_char();
            if (num == 0) break;
            sum += num;
        }

        *ConsoleWriteSInt = sum;
        return 0;
    }

  RISK-XVII assembly:

    00000000 <_start>:
       0:   7ff00113        li    sp, 2047
       4:   00c000ef        jal   ra, 10 <main>
       8:   000017b7        lui   a5, 0x1
       c:   80078623        sb    zero, -2036(a5)

    00000010 <main>:
      10:   000017b7        lui   a5, 0x1
      14:   81678793        addi  a5, a5, -2026
      18:   0007a783        lw    a5, 0(a5)
      1c:   02078c63        beqz  a5, 54 <main+0x44>
      20:   00500713        li    a4, 5
      24:   00000693        li    a3, 0
      28:   00001637        lui   a2, 0x1
      2c:   81660613        addi  a2, a2, -2026
      30:   00f686b3        add   a3, a3, a5
      34:   fff70713        addi  a4, a4, -1
      38:   00070663        beqz  a4, 44 <main+0x34>
      3c:   00062783        lw    a5, 0(a2)
      40:   fe0798e3        bnez  a5, 30 <main+0x20>
      44:   000017b7        lui   a5, 0x1
      48:   80d7a223        sw    a3, -2044(a5)
      4c:   00000513        li    a0, 0
      50:   00008067        ret
      54:   00078693        mv    a3, a5
      58:   fedff06f        j     44 <main+0x34>

===========================================================================
```
