#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define INITIAL_INPUT_SIZE 1000000
#define INITIAL_ROW_SIZE 1000000
#define INITIAL_COMMAND_SIZE 1000000
#define INITIAL_DELETE_SIZE 1000000
#define MINIMUM_STATE_SIZE 16
#define ROW_SIZE 1030

typedef struct InputArray{
    char **inputRows;
    int inputArraySize;
    int inputArrayUsedElements;
}InputArray;

typedef struct RowArray{
    int *rowIndexes;
    int rowArraySize;
    int rowArrayUsedElements;
}RowArray;

typedef struct Command{
    char commandType;
    int firstIndex;
    int lastIndex;
    int firstLine;
    int previousDelete;
    int statePostion;
    int stateSize;
}Command;

typedef struct CommandArray{
    Command *commands;
    int commandArraySize;
    int commandArrayUsedElements;
    int currentCommand;
}CommandArray;

typedef struct DeleteBuffer{
    int **deleteBuffer;
    int deleteBufferSize;
    int deleteBufferUsedElements;
}DeleteBuffer;

int lastDelete;
bool undoOrRedoHappened;

void initializeInputArray(InputArray *iArr, size_t inputSize);
void initializeRowArray(RowArray *rArr, size_t rowSize);
void initializeCommandArray(CommandArray *cArr, size_t commandSize);
void initializeDeleteArray(DeleteBuffer *dBuff, size_t deleteSize);
int addCommand(CommandArray *cArr, char commandType, int firstRow, int lastRow);
void saveState(RowArray *rArr, DeleteBuffer *dBuff, CommandArray *cArr, int command);
int change(int firstRow, int lastRow, InputArray *iArr, int inputIndex, CommandArray *cArr, RowArray *rArr, DeleteBuffer *dBuff);
void print(int firstRow, int lastRow, InputArray *iArr, RowArray *rArr);
void delete(int firstRow, int lastRow, CommandArray *cArr, RowArray *rArr, DeleteBuffer *dBuff);
void undoRedo(CommandArray *cArr, RowArray *rArr, DeleteBuffer *dBuff);
void reChange(int position, CommandArray *cArr, RowArray *rArr);
void computeNewCommandPosition(int initialNumOfCommands, CommandArray *cArr);

int main(){
    InputArray iArr;
    RowArray rArr;
    CommandArray cArr;
    DeleteBuffer dBuff;
    char inputRow[ROW_SIZE];
    char command;
    char *firstArgument;
    char *secondArgument;
    const char *separator = ",";

    iArr.inputRows=NULL;
    rArr.rowIndexes=NULL;
    cArr.commands=NULL;
    dBuff.deleteBuffer=NULL;
    initializeInputArray(&iArr, INITIAL_INPUT_SIZE);
    initializeRowArray(&rArr, INITIAL_ROW_SIZE);
    initializeCommandArray(&cArr, INITIAL_COMMAND_SIZE);
    initializeDeleteArray(&dBuff, INITIAL_DELETE_SIZE);
    lastDelete=0;
    undoOrRedoHappened=false;
    while(strcmp(fgets(inputRow, ROW_SIZE, stdin), "q\n" )!=0){
        if(iArr.inputArrayUsedElements==iArr.inputArraySize){
            iArr.inputArraySize*=2;
            iArr.inputRows=realloc(iArr.inputRows, iArr.inputArraySize*sizeof(char*));
        }
        iArr.inputRows[iArr.inputArrayUsedElements]=calloc(strlen(inputRow)+1, sizeof(char));
        strcpy(iArr.inputRows[iArr.inputArrayUsedElements], inputRow);
        iArr.inputArrayUsedElements++;
    }
    
    for(int i=0; i<iArr.inputArrayUsedElements; i++){
        command=iArr.inputRows[i][strlen(iArr.inputRows[i])-2];
        firstArgument=strtok(iArr.inputRows[i], separator);
        secondArgument=strtok(NULL, separator);
        switch (command){
        case 'c':
            if(undoOrRedoHappened==true){
                undoRedo(&cArr, &rArr, &dBuff);
                cArr.commandArrayUsedElements=cArr.currentCommand+1;
                lastDelete=cArr.commands[cArr.currentCommand].previousDelete;
                undoOrRedoHappened=false;
            }
            i=change(atoi(firstArgument)-1, atoi(secondArgument)-1, &iArr, i+1, &cArr, &rArr, &dBuff);
            break;
        case 'd':
            if(undoOrRedoHappened==true){
                undoRedo(&cArr, &rArr, &dBuff);
                cArr.commandArrayUsedElements=cArr.currentCommand+1;
                lastDelete=cArr.commands[cArr.currentCommand].previousDelete;
                undoOrRedoHappened=false;
            }
            delete(atoi(firstArgument)-1, atoi(secondArgument)-1, &cArr, &rArr, &dBuff);
            break;
        case 'p':
            if(undoOrRedoHappened==true){
                undoRedo(&cArr, &rArr, &dBuff);
                undoOrRedoHappened=false;
            }
            print(atoi(firstArgument)-1, atoi(secondArgument)-1, &iArr, &rArr);
            break;
        case 'u':
            computeNewCommandPosition(-atoi(firstArgument), &cArr);
            break;
        case 'r':
            computeNewCommandPosition(atoi(firstArgument), &cArr);
            break;
        default:
            break;
        }
    }
    return 0;
}

void initializeInputArray(InputArray *iArr, size_t inputSize){
    iArr->inputRows=calloc(inputSize, sizeof(char*));
    iArr->inputArraySize=inputSize;
    iArr->inputArrayUsedElements=0;
}

void initializeRowArray(RowArray *rArr, size_t rowSize){
    rArr->rowIndexes=calloc(rowSize, sizeof(int));
    rArr->rowArraySize=rowSize;
    rArr->rowArrayUsedElements=0;
}

void initializeCommandArray(CommandArray *cArr, size_t commandSize){
    cArr->commands=calloc(commandSize, sizeof(Command));
    cArr->commandArraySize=commandSize;
    cArr->commandArrayUsedElements=0;
    cArr->currentCommand=0;
}

void initializeDeleteArray(DeleteBuffer *dBuff, size_t deleteSize){
    dBuff->deleteBuffer=calloc(deleteSize, sizeof(char*));
    dBuff->deleteBufferSize=deleteSize;
    dBuff->deleteBufferUsedElements=0;
}

int addCommand(CommandArray *cArr, char commandType, int firstRow, int lastRow){
    int commandIndex;
    if(cArr->commandArrayUsedElements==cArr->commandArraySize){
        cArr->commandArraySize*=2;
        cArr->commands=realloc(cArr->commands, cArr->commandArraySize*sizeof(Command));
    }
    cArr->commands[cArr->commandArrayUsedElements].commandType=commandType;
    cArr->commands[cArr->commandArrayUsedElements].firstIndex=firstRow;
    cArr->commands[cArr->commandArrayUsedElements].lastIndex=lastRow;
    commandIndex=cArr->commandArrayUsedElements;
    cArr->currentCommand=cArr->commandArrayUsedElements;
    cArr->commandArrayUsedElements++;
    return commandIndex;
}

void saveState(RowArray *rArr, DeleteBuffer *dBuff, CommandArray *cArr, int command){
    int stateDimension;
    if(dBuff->deleteBufferUsedElements==dBuff->deleteBufferSize){
        dBuff->deleteBufferSize*=2;
        dBuff->deleteBuffer=realloc(dBuff->deleteBuffer, dBuff->deleteBufferSize*sizeof(int*));
    }
    stateDimension=rArr->rowArrayUsedElements;
    cArr->commands[command].stateSize=stateDimension;
    dBuff->deleteBuffer[dBuff->deleteBufferUsedElements]=NULL;
    dBuff->deleteBuffer[dBuff->deleteBufferUsedElements]=calloc(ROW_SIZE, sizeof(int));
    for(int i=0; i<stateDimension; i++){
        dBuff->deleteBuffer[dBuff->deleteBufferUsedElements][i]=rArr->rowIndexes[i];
    }
    dBuff->deleteBufferUsedElements++;
}

int change(int firstRow, int lastRow, InputArray *iArr, int inputIndex, CommandArray *cArr, RowArray *rArr, DeleteBuffer *dBuff){
    int i;
    int commandIndex;
    commandIndex=addCommand(cArr, 'c', firstRow, lastRow);
    cArr->commands[commandIndex].firstLine=inputIndex;
    cArr->commands[commandIndex].statePostion=-1;
    cArr->commands[commandIndex].previousDelete=lastDelete;
    i=0;
    while (strcmp(iArr->inputRows[inputIndex+i], ".\n")!=0){
        if(rArr->rowArrayUsedElements==rArr->rowArraySize){
            rArr->rowArraySize*=2;
            rArr->rowIndexes=realloc(rArr->rowIndexes, (rArr->rowArraySize+1)*sizeof(int));
        }
        if(rArr->rowIndexes[firstRow+i]==0){
            rArr->rowArrayUsedElements++;
        }
        rArr->rowIndexes[firstRow+i]=i+inputIndex;
        i++;
    }
    return i+inputIndex;
}

void print(int firstRow, int lastRow, InputArray *iArr, RowArray *rArr){
    int nullElements;
    nullElements=0;
    if(lastRow==-1){
        fputs(".\n", stdout);
    }else{
        if(firstRow==-1){
            fputs(".\n", stdout);
            firstRow=0;
        }
        for(int i=firstRow; i<=lastRow; i++){
            if(i>=rArr->rowArrayUsedElements||rArr->rowIndexes[i]==0){
                nullElements++;
            }else{
                fputs(iArr->inputRows[rArr->rowIndexes[i]], stdout);
            }
        }
        for(int j=0; j<nullElements; j++){
            fputs(".\n", stdout);
        }
    }
    return;
}

void delete(int firstRow, int lastRow, CommandArray *cArr, RowArray *rArr, DeleteBuffer *dBuff){
    if(firstRow==-1){
        firstRow=0;
    }
    int commandIndex;
    int rowsToDelete;
    commandIndex=addCommand(cArr, 'd', firstRow, lastRow);
    if(lastRow==-1||firstRow>=rArr->rowArrayUsedElements){
        cArr->commands[commandIndex].previousDelete=lastDelete;
        cArr->commands[commandIndex].statePostion=-1;
    }else{
        cArr->commands[commandIndex].previousDelete=commandIndex;
        lastDelete=commandIndex;
        if(lastRow>=rArr->rowArrayUsedElements-1){
            rowsToDelete=rArr->rowArrayUsedElements-firstRow;
            for(int i=0; i<rowsToDelete; i++){
                rArr->rowIndexes[firstRow+i]=0;
            }
            rArr->rowArrayUsedElements-=rowsToDelete;
        }else{
            rowsToDelete=lastRow-firstRow+1;
            for(int i=lastRow+1; i<rArr->rowArrayUsedElements; i++){
                rArr->rowIndexes[i-rowsToDelete]=rArr->rowIndexes[i];
            }
            for(int i=rArr->rowArrayUsedElements-1; i>=rArr->rowArrayUsedElements-rowsToDelete; i--){
                rArr->rowIndexes[i]=0;
            }
            rArr->rowArrayUsedElements-=rowsToDelete;
        }
        saveState(rArr, dBuff, cArr, commandIndex);
        cArr->commands[commandIndex].statePostion=dBuff->deleteBufferUsedElements-1;
    }
}

void undoRedo(CommandArray *cArr, RowArray *rArr, DeleteBuffer *dBuff){
    int previousDelete, statePostion, stateSize;
    if(cArr->commands[cArr->currentCommand].previousDelete==0||cArr->currentCommand==-1){
        for (int i=0; i<rArr->rowArrayUsedElements; i++){
            rArr->rowIndexes[i]=0;
        }
        rArr->rowArrayUsedElements=0;
        previousDelete=-1;

    }
    else{
        previousDelete=cArr->commands[cArr->currentCommand].previousDelete;
        statePostion=cArr->commands[previousDelete].statePostion;
        stateSize=cArr->commands[previousDelete].stateSize;
        free(rArr->rowIndexes);
        rArr->rowIndexes=NULL;
        rArr->rowIndexes=calloc(rArr->rowArraySize, sizeof(int));
        for(int i=0; i<stateSize; i++){
            rArr->rowIndexes[i]=dBuff->deleteBuffer[statePostion][i];
        }
        rArr->rowArrayUsedElements=cArr->commands[previousDelete].stateSize;
    }
    for (int i=previousDelete+1; i<=cArr->currentCommand; i++){
        if(cArr->commands[i].commandType=='c'){
            reChange(i, cArr, rArr);
        }
    }
    
}

void reChange(int position, CommandArray *cArr, RowArray *rArr){
    int charToIns, startPosition, finalPosition;
    charToIns=cArr->commands[position].firstLine;
    startPosition=cArr->commands[position].firstIndex;
    finalPosition=cArr->commands[position].lastIndex;
    for (int i=startPosition; i<=finalPosition; i++){
        if(rArr->rowIndexes[i]==0){
            rArr->rowArrayUsedElements++;
        }
        rArr->rowIndexes[i]=charToIns;
        charToIns++;
    }
}

void computeNewCommandPosition(int initialNumOfCommands, CommandArray *cArr){
    if(cArr->currentCommand+initialNumOfCommands>=cArr->commandArrayUsedElements){
        cArr->currentCommand=cArr->commandArrayUsedElements-1;
    }else{
        if(cArr->currentCommand+initialNumOfCommands<0){
            cArr->currentCommand=-1;
        }else{
            cArr->currentCommand+=initialNumOfCommands;   
        }
    }
    undoOrRedoHappened=true;
}