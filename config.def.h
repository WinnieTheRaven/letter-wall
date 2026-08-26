static char *fontname = "spleen"

static const char CHARSET[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

static const char *PALETTE[] = {
  /*The colors I've selected come from (literally) doom-outrun-electric*/
  "#0c0a20",//background-black
  "#090819",//background-black-alt
  "#131033",//black
  "#1f1147",//brightblack
  "#919ad9",//white
  "#7984D1",//white
  "#f2f3f7",//brightwhite
  "#546A90",//gray
  "#e61f44",//red
  "#cf433e",//brightred
  "#a7da1e",//green
  "#A875FF",//brightgreen
  "#ffd400",//yellow
  "#1ea8fc",//brightblue
  "#3F88AD",//blue
  "#ff2afc",//magenta
  "#df85ff",//brightmagenta
  "#42c6ff",//brightcyan
  "#204052",//cyan    
};

static const Cell *nearpointer[][] = {
"
    \\|/    
   \\ v /   
  \\ \\|/ /  
 \\ \\ v / / 
\\ \\ \\|/ / /
->->-*-<-<-
/ / /|\\ \\ \\
 / / ^ \\ \\ 
  / /|\\ \\  
   / ^ \\   
    /|\\    "
};
