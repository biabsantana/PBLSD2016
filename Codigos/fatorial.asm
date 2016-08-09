.module calc_fatorial
	.pseg
		addi $s0, $zero, 6 		;DEFINE O NUMERO A TER O FATORIAL CALCULADO
		addi $s1, $zero, 1 		;ARMAZENA O RESULTADO
		
		fat:
			addi $sp, $sp, -2 	;Aloca dois espacos na pilha de execucao
			sw $ra, 1($sp) 		;Guarda o endereco de retorno da funcao
			sw $s0, 0($sp) 		;Guarda o N para a chamada da funcao
			
			slti $t0, $s0, 1	;verifica se n < 1
			beq $t0,$zero,call  ;se nao for, o resultado sera 0. Entao pula para o call
			
			addi $v0, $zero, 1  ;caso n < 1, coloca em $v0 o valor 1 (1!)
			addi $sp, $sp, 2    ;libera dois espacos da pilha
			jr $ra 			    ;retorna para $ra
		
		call:
			addi $s0, $s0, -1 	;decrementa o argumento N para (n-1)
			jal fat 		 	;volta para fat, guardando o valor da proxima instrucao (lw $ra, 1($sp), 
							  	;para caso chegue em n < 1 e poder multiplicar
			
			lw $ra, 1($sp)		;pega o ultimo endereco de retorno inserido na pilha
			lw $s0, 0($sp)		;pega o ultimo valor de N inserido na pilha
			addi $sp, $sp, 2	;libera dois espacos da pilha
			
			mul $v0, $v0, $s0	;incrementa a multiplicacao de cada chamada de fat, partindo de 1!
			slt $t0, $fp, $sp	;verifica se todas as chamadas ja foram verificadas
			bne $t0, $zero, write ;finaliza a execucao
			
			jr $ra				;caso nao esteja finalizada, vai para os proximos elementos da pilha
		write:
			sw $v0, 0($gp) 		;salva o resultado na memoria
.end
