# sift

simple interactive fiction text - game engine to create interactive fiction text games working at console

## sift maker

sift maker - program to complie text data to game data

result: `data.sift` file in program's folder

text format for sift maker in EBNF:

```ebnf
game    = screen , { ',' , screen } ;
screen  = trash , text , { point } ;
point   = trash , link , trash , text ; (* aka variant *)
text    = '{' , char , { char } , '}' ;
trash   = { space | comment } ;
comment = '[' , { char } , ']' ;
char    = ? ascii character ? ;
space   = ? white space ascii character ? ;
link    = ? unsigned integer from 0 to 65536 ? ;
```

## sift runtime

sift runtime - program to run games created with sift maker

just open a `.sift` file with the sift runtime

or run this program in folder with a `data.sift` file inside
