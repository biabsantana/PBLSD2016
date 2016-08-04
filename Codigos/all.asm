add $s0, $s1, $s2	;ok
		addu $s0, $s1, $s2	;ok
		clz $s0, $s1		;ok
		clo $s0, $s1		;ok
		sub $s0, $s1, $s2	;ok
		subu $s0, $s1, $s2	;ok
		seb $s0, $s1		;ok
		seh $s0, $s1		;ok
		and $s0, $s1, $s2	;ok
		nor $s0, $s1, $s2	;ok
		wsbh $s0, $s1		;ok
		or $s0, $s1, $s2	;ok
		xor $s0, $s1, $s2	;ok
		ext $s0, $s1, 5, 10 ;ok
		ins $s0, $s1, 5, 10 ;ok
		div $s0, $s1		;ok
		divu $s0, $s1		;ok
		madd $s0, $s1		;ok
		maddu $s0, $s1		;ok
		msub $s0, $s1		;ok
		msubu $s0, $s1		;ok
		mul $s0, $s1, $s2	;ok
    	mult $s0, $s1		;ok
    	multu $s0, $s1		;ok
    	sll $s0, $s1, 5		;ok
    	sllv $s0, $s1, $s2	;ok
    	srl $s0, $s1, 5		;ok
    	sra $s0, $s1, 5		;ok
    	srav $s0, $s1, $s2	;ok
    	srlv $s0, $s1, $s2	;ok
    	rotr $s0, $s1, 5	;ok
    	rotrv $s0, $s1, $s2 ;ok
    	slt $s0, $s1, $s2	;ok
    	sltu $s0, $s1, $s2	;ok
    	movn $s0, $s1, $s2	;ok
    	movz $s0, $s1, $s2	;ok
    	mfhi $s0			;ok
    	mflo $s0			;ok
    	mthi $s0			;ok			
    	mtlo $s0			;ok
    	jr $s0				;ok
    	jalr $s0, $s1		;ok
    	addi $s0, $s1, 5	;ok
    	addiu $s0, $s1, 5	;ok
    	lui $s0, 5			;ok
    	andi $s0, $s1, 5	;ok
    	ori $s0, $s1, 5		;ok
    	xori $s0, $s1, 5	;ok
    	slti $s0, $s1, 5	;ok
    	sltiu $s0, $s1, 5	;ok
    	label:
		beq $s0, $s1, label ;ok
		bgtz $s0, label		;ok
		bne $s0, $s1, label ;ok
		bltz $s0, label		;ok
		lb $s0, 10($s1)		;ok
		lw $s0, 10($s1)		;ok
		lh $s0, 10($s1)		;ok
		sb $s0, 10($s1)		;ok
		sh $s0, 10($s1)		;ok
		sw $s0, 10($s1)		;ok
		j label				;ok
		jal label			;ok
.end
