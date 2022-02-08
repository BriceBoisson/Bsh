foo()
{
    echo $@
}

foo arguments de la fonction
echo $@
foo "d'autres" arguments
exit 4
