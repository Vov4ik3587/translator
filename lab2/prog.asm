.386
.MODEL FLAT, STDCALL

EXTRN	ExitProcess@4:NEAR


;������� ������
.data
	tmp_var	dd	? ;����������, ��� �������������� �����
	tmp_var_eq	dd	? ;����������, ��� ���������
	a	dd ?

	;������������ ���������


.code
	START:
;������������� ��������������� �� ����������
	finit


	CALL ExitProcess@4
END START
