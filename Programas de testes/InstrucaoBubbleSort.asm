.module bubble_sort
	
	.pseg
	
	vetor: .space 20 ;cria um vetor de 20 bytes
	inicio: la   $t0, vetor       ; carrega o endereço base do array em $t0
        li   $t1, 5           ; carrega imediato, $t1 = 5
        sw   $t1, ($t0)       ; primeiro elemento do array é 5
        li   $t1, 2           ; $t1 = 2
        sw   $t1, 4($t0)      ; segundo elemento do array é 2
        li   $t1, 7           ; $t1 = 7
        sw   $t1, 8($t0)      ; terceiro elemento do array é 7
        li   $t1, 1	          ; $t1 = 1	
        sw   $t1, 12($t0)     ; quarto elemento do array é 1
        li   $t1, 6           ; $t1 = 6
        sw   $t1, 16($t0)     ; quinto elemento do array é 6 
	
	
	
	addi $s1, $zero, 5 ;tamanho
	subi $s2, $s1, 1 ;i = tamanho
	addi $s3, $zero, 0 ; j=0
	addi $t1, $zero, 1 ;apenas para comparacao
	addi $aux, $zero, 0 ;auxiliar para troca dos vetores
	
	;Loop externo	
	FirstLoop: 
	sub $s3, $s3, 1
	slt $t0, s2, 1 ; i<1?
	beq $t1, $t0, EndFLopp
		;Loop interno
		ScndLopp: slt $t0 $s3, $s2 ; j<i?
		bne $t1, $t0 FirstLoop
		add $t1, $t0, $s3 ; $t1 recebe a posicao da base do vetor + s3 q eh o j
		sw $aux, 0($t1); aux recebe o valor do vetor da posicao j
		sw $s3($t0), aux ;passa vetor[j] para aux
		add 0($t1), $zero, 1($t1) ;vetor[j] = vetor[j+1]
		sw 1($t1), $aux ;vetor[j+1] = aux
		addi $s3, $zero, 1 ;j++
		j ScndLopp ;repete
	
	EndFLopp
	

.end
