.host
ialloc console, host, cconsole()

class script_class2
{
    property    val, text

    function void say (text param1)
        push         this.val
        push         "This is script_class2: "
        push         param1
        pcall        text, +(text)
        pcall        text, =(text)
        push         console
        push         this.val
        push         true
        pcall        cconsole, print(text, bool)
10000001:
    end
}

class script_class
{
    property    val, text
    property    ss2, script_class2

    function void say (text param1)
        push         this.val
        push         "Using script_class: "
        push         param1
        pcall        text, +(text)
        pcall        text, =(text)
        push         this.ss2
        push         this.val
        scall        script_class2, say(text)
10000002:
    end

    function void on_console_print (text param2)
        push         console
        push         "Script callback called: "
        push         param2
        pcall        text, +(text)
        push         false
        pcall        cconsole, print(text, bool)
10000003:
    end

    function text test_return ()
        palloc       t, text, text()
        push         t
        push         "Hello"
        pcall        text, =(text)
        push         t
        return1      
        jump         10000004

10000004:
    end
}

class Program
{
    property    val, text
    property    sclass, script_class

    function void Main ()
        push         true
        jumpt        5000006
        jump         5000007
5000006:         
        pushctx      
        push         console
        push         this.sclass
        push         "on_console_print(text)"
        pcall        cconsole, on_print(object, text)
        push         this.sclass
        push         "Hello World"
        scall        script_class, say(text)
        popctx       
        jump         5000008
5000007:         
5000008:         
10000005:
    end
}
