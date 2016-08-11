.module calculo_raiz
	
	.pseg
	addi $s0, $zero, 16 ;raiz
	addi $s1, $zero, 1 ;so para comparacao

	
	Loop: slti $t1, $s0, 1 ;verifica se o resultado parcial é menor do que 1
	beq $t1, $s1, Endfor ;caso seja menor do que 1 vai pro final do loop
	addi $s3, $zero, 2
	mul $t0, $s2, $s3 ;indice * 2 + 1
	addi $t0, $t0, 1
	sub $s0, $s0, $t0 ;raiz - indice
	addi $s2, $s2, 1 ;adiciona 1 ao contador
	j Loop
	Endfor: sw $s2, 0($gp) ;armazena o resultado final

.end


;L�GICA
; Fonte: http://www.guj.com.br/t/ajuda-em-assembly/283268
;raiz de 16 = ?
;1º Passo 16 − 1 = 15
;2º Passo 15 − 3 = 12
;3º Passo 12 − 5 = 7
;4º Passo 7 − 7 = 0

;O resultado da raiz quadrada é o contador => 4

;Se chegar em 0 é porque temos uma raiz quadrada perfeita.

;Porque estou utilizando 1,3,5,7 ?

;indice * 2 + 1
;1º Passo => 0 * 2 + 1 => 1
;2º Passo => 1 * 2 + 1 => 3
;3º Passo => 2 * 2 + 1 => 5
