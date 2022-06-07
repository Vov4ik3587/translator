.386
.MODEL FLAT, STDCALL

EXTRN	ExitProcess@4:NEAR


;сегмент данных
.data
	tmp_var	dd	? ;переменная, для преобразования типов
	tmp_var_eq	dd	? ;переменная, для сравнения
	a	dd ?

	;Вещественные константы


.code
	START:
;Инициализпция математического со процессора
	finit


	CALL ExitProcess@4
END START
