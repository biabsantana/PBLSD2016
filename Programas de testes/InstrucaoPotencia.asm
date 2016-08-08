.module calculo_potencia
	
	.pseg
		addi $s0, $zero, 5 ;base
		addi $s1, $zero, 3 ;potencia
		addi $s2, $zero, 1 ;contador
		add $t0, $zero, $s0
	
		Loop: beq $s2, $s1, Endfor
		mul $t0, $t0, $s0
		addi $s2, $s2, 1
		j Loop
		Endfor: sw $t0, 0($gp)
.end
