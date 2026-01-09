Create a project folder in work workspace directory called `*DjCompiler/*`. Inside this directory, create a file called `init.sh` and copy the following contents into that file. This script automates setting up a compiler project.

`init.sh`:

```bash
#!/bin/bash

set -e

# -----------------------------
# 1. Argument Validation
# -----------------------------
if [ "$#" -eq 0 ]; then
    echo "❌ Error: Missing project name."
    echo "Usage: $0 <project_name>"
    exit 1
fi

if [ "$#" -gt 1 ]; then
    echo "❌ Error: Too many arguments. Please provide a single project name."
    echo "Usage: $0 <project_name>"
    exit 1
fi

PROJECT_NAME="$1"

# Check for whitespace in the project name
if [[ "$PROJECT_NAME" =~ [[:space:]] ]]; then
    echo "❌ Error: Project name cannot contain spaces."
    exit 1
fi

echo "💡 Setting up Project: $PROJECT_NAME"

# -----------------------------
# 2. Directory Setup
# -----------------------------
mkdir -p src include test/good test/bad

# Create dummy source files
touch src/"$PROJECT_NAME".l
touch src/"$PROJECT_NAME".y

# Create dummy test files
echo "dummy code" > test/good/simple.txt
echo "dummy code" > test/bad/syntax_error.txt

# -----------------------------
# 3. Makefile Generation
# -----------------------------
echo "🛠  Creating Makefile..."
cat > Makefile << EOF
# -----------------------------
# Configuration
# -----------------------------
PROJECT := $PROJECT_NAME
CC      := gcc
# -Wno-unused-function: lex.yy.c often defines input()/yyunput() which are not always used
CFLAGS  := -Wall -Wextra -Werror -pedantic -std=c99 -g -fsanitize=address \\
           -MMD -MP -Iinclude -Isrc -D_XOPEN_SOURCE=700 \\
           -Wno-unused-function -Wno-unused-parameter

# -----------------------------
# Directories
# -----------------------------
SRC_DIR   := src
BUILD_DIR := build
BIN_DIR   := bin
TEST_DIR  := test
TARGET    := \$(BIN_DIR)/\$(PROJECT)

# -----------------------------
# Sources
# -----------------------------
BISON_SRC := \$(SRC_DIR)/\$(PROJECT).y
FLEX_SRC  := \$(SRC_DIR)/\$(PROJECT).l
BISON_GEN := \$(SRC_DIR)/\$(PROJECT).tab.c
FLEX_GEN  := \$(SRC_DIR)/lex.yy.c

# Main object (the parser, which includes the lexer)
MAIN_OBJ  := \$(BUILD_DIR)/\$(PROJECT).tab.o

# App modules: Any subdirectories in src/ (excluding hidden ones)
APP_SRC_DIRS := \$(shell find \$(SRC_DIR) -mindepth 1 -maxdepth 1 -type d ! -name '.*')
APP_NAMES    := \$(notdir \$(APP_SRC_DIRS))
APP_SRCS     := \$(foreach app,\$(APP_NAMES),\$(SRC_DIR)/\$(app)/\$(app).c)
APP_OBJS     := \$(patsubst \$(SRC_DIR)/%.c,\$(BUILD_DIR)/%.o,\$(APP_SRCS))

# -----------------------------
# Dependencies
# -----------------------------
DEPS := \$(MAIN_OBJ:.o=.d) \$(APP_OBJS:.o=.d)

# -----------------------------
# Default Target
# -----------------------------
all: \$(TARGET)

# -----------------------------
# 1. Generators (Flex/Bison)
# -----------------------------
\$(BISON_GEN): \$(BISON_SRC)
	@echo "🦬 Running Bison..."
	bison \$(BISON_SRC) -o \$(BISON_GEN)
	@echo "🔧 Patching \$(PROJECT).tab.c..."
	sed -i '/extern YYSTYPE yylval/d' \$(BISON_GEN)

\$(FLEX_GEN): \$(FLEX_SRC) \$(BISON_GEN)
	@echo "💪 Running Flex..."
	flex -o \$(FLEX_GEN) \$(FLEX_SRC)

# -----------------------------
# 2. Compile Rules
# -----------------------------
\$(MAIN_OBJ): \$(BISON_GEN) \$(FLEX_GEN)
	@mkdir -p \$(dir \$@)
	\$(CC) \$(CFLAGS) -c \$(BISON_GEN) -o \$@

# Other modules
\$(BUILD_DIR)/%.o: \$(SRC_DIR)/%.c
	@mkdir -p \$(dir \$@)
	\$(CC) \$(CFLAGS) -c \$< -o \$@

# -----------------------------
# 3. Link
# -----------------------------
\$(TARGET): \$(MAIN_OBJ) \$(APP_OBJS)
	@mkdir -p \$(BIN_DIR)
	@echo "🔗 Linking \$(TARGET)..."
	\$(CC) \$(CFLAGS) \$^ -o \$@

# -----------------------------
# 4. Test Infrastructure
# -----------------------------
test: \$(TARGET)
	@echo "🧪 Running Tests..."
	@passed=0; total=0; failed=0; \\
	echo "--- Good Cases (Expect Success) ---"; \\
	for file in \$(TEST_DIR)/good/*; do \\
		[ -e "\$\$file" ] || continue; \\
		total=\$\$((total+1)); \\
		if ./\$(TARGET) "\$\$file" > /dev/null 2>&1; then \\
			passed=\$\$((passed+1)); \\
		else \\
			failed=\$\$((failed+1)); \\
			echo "❌ FAILED: \$\$file"; \\
			echo "   Output:"; \\
			./\$(TARGET) "\$\$file"; \\
			echo "-------------------------"; \\
		fi; \\
	done; \\
	echo "--- Bad Cases (Expect Failure) ---"; \\
	for file in \$(TEST_DIR)/bad/*; do \\
		[ -e "\$\$file" ] || continue; \\
		total=\$\$((total+1)); \\
		if ./\$(TARGET) "\$\$file" > /dev/null 2>&1; then \\
			failed=\$\$((failed+1)); \\
			echo "❌ FAILED (Unexpected Success): \$\$file"; \\
		else \\
			passed=\$\$((passed+1)); \\
		fi; \\
	done; \\
	echo ""; \\
	echo "📊 Result: \$\$passed / \$\$total passed."; \\
	if [ \$\$failed -ne 0 ]; then exit 1; fi

# -----------------------------
# Utilities
# -----------------------------
clean:
	@echo "🧹 Cleaning..."
	rm -rf \$(BUILD_DIR) \$(BIN_DIR) \$(BISON_GEN) \$(FLEX_GEN)

-include \$(DEPS)

.PHONY: all clean test
EOF

# -----------------------------
# 4. Wrapper Script Generation
# -----------------------------
echo "🚀 Generating wrapper script: $PROJECT_NAME.sh"
cat > "$PROJECT_NAME.sh" << EOF
#!/bin/bash
# Rebuild silently
make -s all
# Run the binary with arguments
./bin/$PROJECT_NAME "\$@"
EOF

chmod +x "$PROJECT_NAME.sh"

echo "✅ Project '$PROJECT_NAME' initialized!"
echo "   - Usage: ./$PROJECT_NAME.sh <input_file>"

# -----------------------------
# 5. Self Destruct
# -----------------------------
echo "💥 Self-destructing init script..."
rm -- "$0"
```

Make it executable using the `chmod` command:

```bash
**…/DjCompiler ❯** chmod +x init.sh
```

Then we can use the `bash` command to run the script and pass the language name as an argument like so:

```bash
**…/DjCompiler ❯** bash init.sh dj
```
