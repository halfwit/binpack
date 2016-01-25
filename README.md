For use with dotfiles/local/cfg/wmutils/wshuf
eg: wshuf | binpack -x <your screen width> -y <your screen height> -g <gaps> | while read r x y w h; do
        wtp "$x" "$y" "$w" "$h" "$wid" 
    done
    
tested with clang -o binpack binpack.c -std=c99 
