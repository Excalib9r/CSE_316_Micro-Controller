.MODEL SMALL 
.STACK 100H 
.DATA
 N DW 0
 COUNT DW 0

.CODE 
MAIN PROC 
    MOV AX, @DATA
    MOV DS, AX
    
    
    XOR CX, CX ; counter
    XOR DX, DX ; store result  
    
    
    
    
    DECIMAL_INPUT PROC
        FOR: 
        
           MOV AH, 1
           INT 21H
           CMP AL, 0DH
           JE END_FOR
           SUB AL, 48
           MOV AH, 0
           MOV N, AX
           MOV AX, 10
           MUL DX
           ADD AX, N
           MOV DX, AX
           INC COUNT
           JMP FOR  
           
        END_FOR:  
        
    RET
     
    DECIMAL_INPUT ENDP 
        
   

	; interrupt to exit
    MOV AH, 4CH
    INT 21H
    
  
MAIN ENDP 
END MAIN 


