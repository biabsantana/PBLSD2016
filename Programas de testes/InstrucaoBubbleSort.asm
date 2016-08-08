.module bubble_sort
	
	.pseg

	
	Start: lw $s0, 0($gp) ; carrega o endereço base do array
	addi $t0, $zero, 5 ;tamanho
	move $s1, $t0 ; $s1 = i
	addi $s1, $s1, -1 ;tamanho - 1
	move $s2, $zero ;j=0
	addi $s4, $zero, 1
	
	FstLoop: 
			 move $a2, $gp
			 slti $a0, $s1, 2;i>=1?
			 bne $a0, $zero, End
		ScndLoop: 
				  slt $t1, $s2, $s1 ; j<i?
				  bne $t1, $s4, DecrementaI
				  ;if
				  lw $t2, 0($a2)
				  lw $t3, 1($a2)
				  slt $t4, $t3, $t2 ;vetor[i]>vetor[i+j]?
				  bne $t4, $zero, Swap
				  j IncrementJ

					
		Swap: 	;move $s3, $t2 ;aux = vetor[j]
				sw $t3, 0($a2) ;vetor[j] = vetor[j+1]
				sw $t2, 1($a2)
				
		
		IncrementJ: addi $s2, $s2, 1 ;j++
					addi $a2, $a2, 1
					j ScndLoop
					
		DecrementaI: addi $s1, $s1, -1 
					j FstLoop
		
		End: 
				  
	.dseg
	
		vetor: ;cria um vetor
		  .word 5
		  .word 2
		  .word 7
		  .word 1
		  .word 6 
.end
