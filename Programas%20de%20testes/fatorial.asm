.module calc_fatorial
	.pseg
fat: 
	addi $sp, $sp, 2		;alocando 2 espaços na pilha
	sw $ra, 1($sp)			
	sw $s0, 0($sp)
	slti $t0, $s0, 1		; comparação se é menor que 1
	beq $t0, $zero, Loop	; vai a Loop 
	addi $s2, $zero, 1		; coloca 1 como variavel de retorno
	addi $sp, $sp, -2		; apaga a pilha 
	jr $ra
	
Loop: 
	addi $s0, $s0, -1  		; verificar função
	jal fat					; duvida na implementação desta parte por fazer uma chama do metodo
	lw $s0, 0($sp)			
	lw $ra, 1($sp)
	addi $sp, $s0, -2		;apagando a pilha 
	mul $s2, $s0, $s2		; produtorio e armazenamento em s2	
	jr $ra 					; retorno
	
.end
