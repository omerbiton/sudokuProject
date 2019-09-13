#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
/*structure to represent a stack*/
struct Stack {
    int top;
    unsigned capacity;
    int* array;
};

/* create a stack of a given capacity. It initializes size of
 * stack as 0
 * */
struct Stack* createStack(unsigned capacity)
{
    struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
    checkAllocatedMemory("malloc", stack);
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (int*)malloc(stack->capacity * sizeof(int));
    checkAllocatedMemory("malloc", stack->array);
    return stack;
}Stack;

// Stack is full when top is equal to the last index
int isFull(Stack* stack)
{
    return stack->top == stack->capacity - 1;
}

// Stack is empty when top is equal to -1
int isEmpty(Stack* stack)
{
    return stack->top == -1;
}

// Function to add an item to stack.  It increases top by 1
void push(Stack* stack, int item)
{
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
}

// Function to remove an item from stack.  It decreases top by 1
int pop(Stack* stack)
{
    if (isEmpty(stack))
        return INT_MIN;
    return stack->array[stack->top--];
}

// Function to return the top from stack without removing it
int peek(Stack* stack)
{
    if (isEmpty(stack))
        return INT_MIN;
    return stack->array[stack->top];
}
