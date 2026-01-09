Now we have a working compiler!

The code generator compiles for us `program.asm` assembly file. We can use the NASM assembler and the standard linker (`ld`) to assemble, link and execute the program:

1. **Assemble:**
    
    ```bash
    nasm -f elf64 program.asm -o program.o
    ```
    
2. **Link (Pure):**
    
    ```bash
    ld program.o -o program
    ```
    
3. Now you can what your programs run!
    
    ```bash
    ./bin/program
    ```
    

---

However, it is a little inconvenient for developers to have to run these commands everytime they want to test a program. Instead, we can have our compiler program do this automatically.

First, include the `<stdlib.h>` library in the `%code provides{ … }` section at the top of our Bison file, `dj.y`:

```c
#include <stdlib.h> // Required for system()
```

Then, add this logic to the bottom of our Bison file, `dj.y`, in the main function after you call `generateNASM` & `fclose`:

```c
int main(int argc, char *argv[]) {
	// ... (Your existing code) ...

  // Execute the Build Commands
  int nasmStatus = system("nasm -f elf64 program.asm -o program.o");
  if (nasmStatus != 0) {
      fprintf(stderr, "Error: NASM failed to assemble.\n");
      return 1;
  }

  int ldStatus = system("ld program.o -o program");
  if (ldStatus != 0) {
      fprintf(stderr, "Error: Linker failed.\n");
      return 1;
  }

  // Run the generated program
  int runStatus = system("./program");
  printf("\n--- Program exited with code: %d ---\n", runStatus);

  return 0;
}
```
