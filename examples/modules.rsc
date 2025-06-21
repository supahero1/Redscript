use lang;
method z()
{
    
}

module test
{
    method z()
    {
        msg(@r, "in z!");
    }
    method x()
    {
        method y()
        {
            msg(@r, "in y!");
        }
        ::z(); // z();

        ::y();
    }
}
method x()
{
    method y()
    {
        msg(@r, "In y (not in module y)!");
    }
    y();
}

test::x();
::test::z();

::x();
// outputs: In z! - In y! - In z! - In y (not in module y)!

