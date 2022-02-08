a=12
(echo $a; a=13; (echo $a; a=42; echo $a); echo $a)
echo $a
