.module fibonacci
	.pseg
		li $s0, 15 					;QUANTIDADE DE NUMEROS NA SEQUENCIA DE FIB
		move $t2, $gp				;Endereco de armazenamento
		
		FIB:
			slti $t0, $s0, 2		;condicao base n < 2
			bne $t0, $zero, VERIFY	;caso seja a condicao base
			addi $t0, $s0, -1		;caso nao seja (n-1)
			addi $t1, $s0, -2		;(n-2)
			add $t0, $t0, $t1		;(n-1) + (n-2)
			
		VERIFY:
			sw $t0, 0($t2)			;coloca na memoria e continua o processo
			addi $t2, $t2, 1
			addi $s0, $s0, -1
			bne $s0, $zero, FIB
			
.end
