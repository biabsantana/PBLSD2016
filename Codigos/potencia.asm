.module calculo_potencia
	
	.pseg
		addi $s0, $zero, 7 		   ;DEFINE A BASE
		addi $s1, $zero, 3 		   ;DEFINE A POTENCIA
		addi $s2, $zero, 1 		   ;CONTADOR DO LOOP
		add $t0, $zero, $s0		   ;atribui a primeira iteracao, colocando a base em $t0
	
		Loop: beq $s2, $s1, Endfor ;verifica se o contador do loop e igual a potencia, ou seja, fim de calculo
		mul $t0, $t0, $s0		   ;multiplica a base por ela mesmo e armazena em $t0
		addi $s2, $s2, 1		   ;incrementa contador
		j Loop					   ;volta para o loop, caso contador != potencia
		
		Endfor: 
		sw $t0, 0($gp)			   ;guarda o resultado na memoria
.end
