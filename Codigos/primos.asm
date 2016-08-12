.module primos
	.pseg
	
		li $s4, 15						;QUANTIDADE DE NUMEROS PRIMOS(entrada)
		li $s0, 2						;dividendo
		move $s1, $s0					;divisor
		li $s3, 2						;verifica se e primo(apenas duas divisoes com resto 0)
		move $t2, $gp					;pega o valor do gp para armazenar os numeros primos
				
		LOOP:
			div $s0, $s1				;(dividendo/divisor) atuais
			mfhi $t0					;pega o resto
			bne $t0, $zero, CONTINUE	;se o resto for diferente de zero, nao e primo
			addi $t1, $t1, 1			;se for, incrementa o valor de divisoes certas
			
		CONTINUE:	
			addi $s1, $s1, -1			;decrementa o divisor
			beq $zero, $s1, VERIFY		;se divisor for igual a 0
			j LOOP						;se nao for, realizara mais uma divisao
			
		VERIFY:
			bne $s3, $t1, END			;se nao houver apenas 2 divisoes corretas(nao e primo)
			addi $s4, $s4, -1			;se for, contabiliza mais um numero primo na quantidade informada
			sw $s0, 0($t2)				;armazena na memoria
			addi $t2, $t2, 1			;incrementa a posicao da memoria para um possivel numero primo
		
		END:
			beq $s4, $zero, ENDFOR		;se for o ultimo valor a ser verificado, encerra o programa
			addi $s0, $s0, 1			;se nao for, incrementa o dividendo
			
			;reseta valores
			move $s1, $s0
			move $t1, $zero
			
			j LOOP
				
		ENDFOR:
			;nope
.end
