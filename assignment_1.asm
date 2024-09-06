.MODEL SMALL 
.STACK 100H 
.DATA

.CODE 
MAIN PROC 
            
    MOV AH, 1 
    MOV DL, 123
    MOV AL, 123
     
    
    REPEAT:
    INT 21H 
    
    CMP DL, AL 
    JNBE CHANGE 
    
    JMP ENTER_LOOP
    
    CHANGE:
    MOV DL, AL
    
    ENTER_LOOP:
    
    
    CMP AL, 'a' 
    JNGE END_REPEAT 
    
    CMP AL, 'z'
    JNLE END_REPEAT
    
    JMP REPEAT 
    
    END_REPEAT:
    MOV AH, 2
    INT 21H
     
	; interrupt to exit
    MOV AH, 4CH
    INT 21H
    
  
MAIN ENDP 
END MAIN 


