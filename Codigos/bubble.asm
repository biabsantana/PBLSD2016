.module bubble
	.pseg
		addi $s0, $zero, 8 	;TAMANHO DO VETOR
		move $s1, $zero		;ITERADOR DO LOOP EXTERNO
		move $s2, $zero		;ITERADOR DO LOOP INTERNO
		addi $s4, $s0, -2
		
		LoopExterno:
		
			move $s3, $gp ;recebe o atual primeiro elemento do array
			
			LoopInterno:
				lw $t3, 0($s3) ;pega j
				lw $t4, 1($s3) ;pega j+1
		
				slt $t2, $t4, $t3 
				beq $t2, $zero, NaoTroca
	
				sw $t3, 1($s3)
				sw $t4, 0($s3)
				
				NaoTroca:
					slt $t2, $s2, $s4
					beq $t2, $zero, SaiLoopInterno
					addi $s2, $s2, 1
					addi $s3, $s3, 1
					j LoopInterno
				
					SaiLoopInterno:
						move $s2, $zero
			
			addi $s1, $s1, 1
			slt $t2, $s1, $s0
			bne $t2, $zero, LoopExterno	
				
	
	.dseg
		.word 10
		.word 5
		.word 3
		.word 2
		.word 77
		.word 47
		.word 105
		.word 1000
.end
