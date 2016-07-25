.module calculo_potencia
	.dseg
	
	resFinal: 
			.word 0
	res: 
		.word 0
	
	.pseg
	li $numero, 2
	li $elevado, 4
	li $cont, 1
	move $res, $x
	

	Loop: beq, $cont, $elevado, Endfor
	mul $res, $res, $numero
	add $cont, $cont, 1
	j Loop
	Endfor: sw resFinal, $res
	
.end