package main

import (
	"ethos/elScript"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"os"
)

func main() {
	in := flag.String("in", "", "source dir")
	out := flag.String("out", "", "destination dir")
	flag.Parse()

	if *in == "" || *out == "" {
		log.Fatalf("need both --in and --out directories")
	}

	files, err := ioutil.ReadDir(*in)
	if err != nil {
		log.Fatalf(err.String())
	}

	for _, file := range files {
		content, err := ioutil.ReadFile(*in + "/" + file.Name)
		if err != nil {
			fmt.Printf("Error reading %s: %s\n", file.Name, err.String())
			continue
		}

		outName := *out + "/" + file.Name
		f, err := os.Open(outName)
		if err != nil {
			f, err = os.Create(outName)
			if err != nil {
				fmt.Printf("Error creating %s: %s\n", outName, err.String())
				continue
			}
		}
		script := &elScript.ElScript{Content: string(content)}
		encoder := elScript.NewEncoder(f)
		encoder.ElScript(script)
		encoder.Flush()
	}
}
