.module calculo_potencia
	.dseg
	
	resFinal: 
			.word 0
	
	.pseg
	li $s0, 2 ;base
	li $s1, 4 ;potencia
	li $s2, 1 ;contador
	move $t0, $s0
	

	Loop: beq, $s2, $s1, Endfor
	mul $t0, $t0, $s0
	add $s2, $s2, 1
	j Loop
	Endfor: sw resFinal, $t0
	
.end