#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
/*structure to represent a stack*/
struct Stack {
    int top;
    unsigned capacity;
    int* array;
}Stack;

/* create a stack of a given capacity. It initializes size of
 * stack as 0
 * */
Stack* createStack(unsigned capacity)
{
    struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
    checkAllocatedMemory("malloc", stack);
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (int*)malloc(stack->capacity * sizeof(int));
    checkAllocatedMemory("malloc", stack->array);
    return stack;
}

/* Stack is full when top is equal to the last index */
int isFull(Stack* stack)
{
    return stack->top == stack->capacity - 1;
}

/* Stack is empty when top is equal to -1 as in the begining */
int isEmpty(Stack* stack)
{
    return stack->top == -1;
}

/* Increasing top by 1 and then add an item to stack at the new top position */
void push(Stack* stack, int item)
{
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
}

/* Decreasing top by 1 and then remove an item from stack at the new top position */
int pop(Stack* stack)
{
    if (isEmpty(stack))
        return INT_MIN;
    return stack->array[stack->top--];
}

/* return the top item from the stack without removing it */
int peek(Stack* stack)
{
    if (isEmpty(stack))
        return INT_MIN;
    return stack->array[stack->top];
}
