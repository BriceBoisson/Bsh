fun()
{
    echo "First fun"
}

(fun; fun() { echo "Second fun"; }; fun;)
fun;
