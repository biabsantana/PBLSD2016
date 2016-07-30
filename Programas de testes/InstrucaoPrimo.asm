.module calculo_primo
	
	.pseg
	
	li $a0, 1
	addi $s0, $a0, 10 ;numero limite + 1 (para comparacao de menor ou igual)
	addi $t0, $zero, 1 ;contador
	
	slt $t1, $t0, s0  ; s0<=1?
	bqe $t1, $zero, Fim ;se NÃO for menor fim do programa
	
	addi $a1, $zero, 2
	slt $t1, $a1, $s0 ;o numero limite < 2?
	bne $t1, $a0, Verifica ;se nao for, pula o passo da impressao
	li $v0, 1 ;codigo para imprimir um inteiro
	la $a2, ($s0) ;$a2 eh o valor a ser impresso
	syscall ;executa a chamado do SO para imprimir
	j Fim ;fim do programa ele eh o unico primo
	
	Verifica: slt $t1, $t0, s0  ; s0<=1?
	bqe $t1, $zero, Fim ;se NÃO for menor fim do programa
	div $s2, $s0, $t0 ;divide o numero limite pelo contador (x/i)
	mfhi $a1 ;pega o resto
	beq	$a1, $zero, Conta ;se o resto for zero eh primo
	j Verifica
	
	Conta: 	addi $t0, $t0, 1 ;add 1 no contador
			li $v0, 1 ;codigo para imprimir um inteiro
			la $a2, ($s0) ;$a2 eh o valor a ser impresso
			syscall ;executa a chamado do SO para imprimir
			j Verifica ;volta pra ver os outros numeros
	
	Fim: 	li $v0, 10	; código para encerrar o programa
			syscall		; executa a chamada do SO para encerrar
	
			
.end
