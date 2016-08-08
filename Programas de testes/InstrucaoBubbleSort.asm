.module bubble_sort
	
	.pseg

	
	inicio: sw $t0, 0($gp)       ; carrega o endereço base do array em $t0
   	
	addi $s1, $zero, 5 ;tamanho
	addi $t9, $zero, 1
	sub $s2, $s1, $t9 ;i = tamanho
	addi $s3, $zero, 0 ; j=0
	addi $t1, $zero, 1 ;apenas para comparacao
	addi $s0, $zero, 0 ;auxiliar para troca dos vetores
	
	;Loop externo	
	FirstLoop: 
	sub $s3, $s3, 1
	slti $t0, $s2, 1 ; i<1?
	beq $t1, $t0, EndFLopp
		;Loop interno
		ScndLopp: slt $t0, $s3, $s2 ; j<i?
		bne $t1, $t0, FirstLoop
		add $t1, $t0, $s3 ; $t1 recebe a posicao da base do vetor + s3 q eh o j
		add $s0, $zero, $t1 ; aux recebe o valor do vetor da posicao j
		add $t1, $t1, 1
		add $t1, $zero, $t1 ;vetor[j] = vetor[j+1]
		addi $t1, $t1, 1
		add $s0, $zero, $t1 ;vetor[j+1] = aux
		addi $s3, $zero, 1 ;j++
		j ScndLopp ;repete
	
	EndFLopp:
	
	.dseg
	
		vetor: ;cria um vetor
		  .word 5
		  .word 2
		  .word 7
		  .word 1
		  .word 6 
.end
