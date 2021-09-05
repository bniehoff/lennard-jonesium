import lennardjones as lj

def test_hello(capfd):
    lj.hello("Alice")
    captured = capfd.readouterr()
    assert captured.out == "Hello, Alice, and welcome to Cython!\n"
