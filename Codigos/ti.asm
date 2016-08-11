.module
	.pseg
		
		addi $t1, $zero, 1
		sll $t1, $t1, 1
		srl $t1, $t1, 1
		
		addi $t1, $zero, 1
		sllv $t1, $t1, $t1
		srlv $t1, $t1, $t1
		
.end
