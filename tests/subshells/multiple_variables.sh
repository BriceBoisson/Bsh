a=12
b=13
c=14
d=15
e=16

echo "$a $b $c $d $e"
(echo "$a $b $c $d $e"; a=21; c=41; e=61; echo "$a $b $c $d $e";)
echo "$a $b $c $d $e";
