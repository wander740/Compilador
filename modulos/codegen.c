/* codegen.c */
#include "output.h"
#include "codegen.h"

/* carrega uma constante no registrador primário */
void asmLoadConstant(char *s)
{
    emit("MOV AX, %s", s);

}

/* carrega uma variável no registrador primário */
void asmLoadVariable(char *s)
{
    emit("MOV AX, [%s]", s);

}

/* inverte sinal de reg. prim. */
void asmNegate()
{
    emit("NEG AX");

}

/* coloca registrador primário na pilha */
void asmPush()
{
	emit("PUSH AX");

}

/* adiciona topo da pilha a reg. primário */
void asmPopAdd()
{
	emit("POP BX");
	emit("ADD AX, BX");

}

/* subtrari do topo da pilha o reg. primário */
void asmPopSub()
{
	emit("POP BX");
	emit("SUB AX, BX");
	asmNegate();

}

/* adiciona topo da pilha a reg. primário */
void asmPopMul()
{
    emit("POP BX");
    emit("IMUL BX");

}

/* subtrari do topo da pilha o reg. primário */
void asmPopDiv()
{
    emit("POP BX");
    emit("XCHG AX, BX");
    emit("CWD");
    emit("IDIV BX");

}

/* armazena valor do registrador primário em variável */
void asmStoreVariable(char *name)
{
    emit("MOV [%s], AX", name);

}

/* aplica OU com topo da pilha a reg. primário */
void asmPopOr()
{
    emit("POP BX");
    emit("OR AX, BX");

}

/* aplica OU-exclusivo com topo da pilha a reg. primário */
void asmPopXor()
{
    emit("POP BX");
    emit("XOR AX, BX");

}

/* aplica AND com topo da pilha a reg. primário */
void asmPopAnd()
{
    emit("POP BX");
    emit("AND AX, BX");

}

/* aplica NOT a reg. primário */
void asmNot()
{
    emit("NOT AX");

}