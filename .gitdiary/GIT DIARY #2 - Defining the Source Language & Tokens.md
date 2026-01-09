Here we will take an example language called Diminished Java (DJ). DJ is a small programming language similar to Java. DJ has been designed to try to satisfy two opposing goals:

1. DJ is a complete object-oriented programming language (OOPL): (a) DJ includes all the core features of OOPLs like Java, and (b) you can express any algorithm in DJ (more precisely, DJ is Turing complete; any Turing machine can be encoded as a DJ program).
2. DJ is simple, with only core features included. DJ can therefore be compiled straightforwardly; we can design and implement a working (non-optimizing but otherwise complete) DJ compiler in one semester.

### Define Source Language

*See below for a complete DJ definition:*

[DJ-definition.pdf](attachment:2a3d9e4c-5dbd-4e56-9c50-bc6fc0780a3c:DJ-definition.pdf)

**Example 1:**

```java
class C extends Object { }
main {
 0;
}
```

**Example 2:**

```java
main {0;}
```

**Example 3:**

```java
//prints 4
main {
  nat x; 
  x=0; 
  x=x+1;
  x=x+1; 
  x=x+1;
  x=x+1; 
  printNat(x);
}
```

**Example 4:**

```java
//prints 4
main {
  nat x; 
  ((x=0)); 
  (x=((x)+(1)));
  x=x+1;
  (x=x+1);
  x=((x)+1);
  (printNat(((x))));
}
```

**Example 5:**

```java
// This DJ program outputs the sum 1 + 2 + ... + 100
class Summer extends Object {
    
  // This method returns the sum 0 + 1 + .. + n
  nat sum(nat n) {
    nat toReturn;
    // note: nat variables automatically get initialized to 0
      
    while(0<n) { 
        toReturn = toReturn + n; 
        n = n - 1;
    };

    toReturn;
  }
}

main {
  // create a new object of type Summer
  Summer s;
  s = new Summer();

  // print the sum 0 + 1 + ... + 100
  printNat( s.sum(100) );
}
```

**Example 6:**

```java
// This program tests the short-circuit || operation.
// The correct output is:
//  3
//  987
//  4
//  987
//  4
//  987

class Tester extends Object {
  nat print4return1(nat unused) { printNat(4); 1; }
}

main {
  Tester test;
  test = new Tester();

  if(0 || 0) {printNat(987);}
  else {printNat(3);};

  if(1 || test.print4return1(0)) {printNat(987);}
  else {printNat(3);};

  if(test.print4return1(0) || 0) {printNat(987);}
  else {printNat(3);};

  if(0 || test.print4return1(0)) {printNat(987);}
  else {printNat(3);};
}
```

**Example 7:**

```java
//This DJ program inputs a natural number n 
//and then outputs n factorial, which is
//calculated in a recursive function

class Factorial extends Object {
  //  Returns n!   
  nat factorial(nat n) {
    if(1<n) {n*factorial(n-1);}
    else {1;};
  }
}

main {
  Factorial f;
  f = new Factorial();
  printNat(f.factorial(readNat()));
}
```

**Example 8:**

```java
//prints 2
class C1 extends Object { 
  nat callWhoami(nat unused) {this.whoami(0);}
  nat whoami(nat unused) {printNat(1);}
}
class C2 extends C1 { 
  nat whoami(nat unused) {printNat(2);} 
}
main {
  C1 testObj;
  testObj = new C2();
  testObj.callWhoami(0);
}
```

**Example 9:**

```java
//Test mutually recursive classes, initializations, the
//dot operator, and dynamic (virtual) method calls.

//Correct output:
//  1
//  2
//  4
//  5
//  6
//  1
//  2
//  4
//  4
//  0
//  33

class C1 extends Object {
  nat v1;
  C2 v2;
  Object init(nat unused) { v1 = 1024; v2 = new C2(); }
  //print the class to which this object belongs 
  nat whoami(nat unused) { printNat(1); }
  nat testWho(C1 o) { o.whoami(0); }
}
class C2 extends C1 {
  C3 v3;
  Object init(nat unused) { v3 = new C3(); }
  nat whoami(nat unused) { printNat(2); }
}
class C3 extends Object {
  C4 v4;
  Object init(nat unused) { v4 = new C4(); }
  nat whoami(nat unused) { printNat(3); }
}
class C4 extends C2 {
  C5 v5;
  Object init(nat unused) { v5 = new C5(); }
  nat whoami(nat unused) { printNat(4); }
}
class C5 extends C2 {
  C6 v6;
  Object init(nat unused) { v6 = new C6(); }
  nat whoami(nat unused) { printNat(5); }
}
class C6 extends C4 {
  C1 v7;
  Object init(nat unused) { v7 = new C1(); }
  nat whoami(nat unused) { printNat(6); }
}
main {
  C1 c;
  c = new C1();
  c.init(0);
  c.v2.init(0);
  c.v2.v3.init(0);
  c.v2.v3.v4.init(0);
  c.v2.v3.v4.v5.init(0);
  c.v2.v3.v4.v5.v6.init(0);
  c.v2.v3.v4.v5.v6.v7.init(0);
  c.v2.v3.v4.v5.v6.v7.v2.init(0);
  c.v2.v3.v4.v5.v6.v7.v2.v3.init(0);
  c.testWho(c);
  c.testWho(c.v2);
  c.testWho(c.v2.v3.v4);
  c.testWho(c.v2.v3.v4.v5);
  c.testWho(c.v2.v3.v4.v5.v6);
  c.testWho(c.v2.v3.v4.v5.v6.v7);
  c.testWho(c.v2.v3.v4.v5.v6.v7.v2);
  c.testWho(c.v2.v3.v4.v5.v6.v7.v2.v3.v4);
  c.v2.v3.v4.v5.v6.v7.v2.v3.v4.testWho(c.v2.v3.v4.v5.v6.v7.v2.v3.v4);
  c.v2.v3.v4.v1 = 33;
  printNat(c.v2.v1);
  printNat(c.v2.v3.v4.v1);
}
```

**Example 10:**

```java
//Reads natural numbers, inserting them into a binary search tree.
//Stops reading after a 0 is read.  At that point, the list of 
//numbers is printed via an inorder tree traversal.

//The net effect is that a list of numbers gets read, sorted, and printed 
//in ascending order.

class BST extends Object {
  nat data;
  BST left;
  BST right;

  //set the data value in the tree's root node to newData
  //returns newData
  nat setData(nat newData) { data=newData; }

  //insert newData into the tree
  //always returns 0
  nat insert(nat newData) {
    BST newRight;
    BST newLeft;
    if(data < newData) {
      //insert into right subtree
      if(right==null) {
        newRight = new BST();
        newRight.data = newData;
        right = newRight;
        0;
      }
      else { right.insert(newData); };
    }
    else {
      //insert into left subtree
      if(left==null) {
        newLeft = new BST();
        newLeft.data = newData;
        left = newLeft;
        0;
      }
      else { left.insert(newData); };
    };
    0;
  }

  //print the tree's data in order
  //always returns 0
  nat printTreeInorder(nat unused) {
    if(left==null) {0;} else{left.printTreeInorder(0);};
    printNat(data);
    if(right==null) {0;} else{right.printTreeInorder(0);};
  }
}

main {
  nat input;
  //create a tree
  BST bst;
  bst = new BST();

  //read data into the tree
  input = readNat();
  bst.setData(input);
  while (!(input==0)) {
    input = readNat();
    bst.insert(input);
  };

  //print the tree in order
  bst.printTreeInorder(0);
}
```

### Create the Project

In the project directory create a test folder. In the test folder have a *good/* folder to store examples that should pass and *bad/* folder to store example that you do not want to pass.

```
📂 dj-compiler/
├── 📂 src/
│   └── ...
├── 📂 tests/
│   ├── 📂 good/
│   |   └── 📄 good1.dj
│   └── 📂 bad/
│       └── 📄 bad1.dj
├── 📄 .gitignore
├── 📄 LICENSE
└── 📄 README.md
```

### Define Tokens

Based on your examples, define some tokens. To make a minimum viable version, you only need tokens used for defining methods, basic operators (e.g. PLUS, MINUS, TIMES) and basic statements (IF, WHILE). 

```c
	FINAL
	CLASS
	ID
	EXTENDS
	MAIN
	NATTYPE
	NATLITERAL
	PRINTNAT
	READNAT
	PLUS
	MINUS
	TIMES
	EQUALITY
	LESS
	ASSERT
	OR
	NOT
	IF
	ELSE
	WHILE
	ASSIGN
	NUL
	NEW
	THIS
	DOT
	SEMICOLON
	LBRACE
	RBRACE
	LPAREN
	RPAREN
	ENDOFFILE
```
