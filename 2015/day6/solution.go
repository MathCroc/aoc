package main

import (
	"fmt"
	"os"
	"io/ioutil"
	"strings"
	"regexp"
	"strconv"
)

func simulate(input []byte, on func(int), off func(int), toggle func(int)) {
	strs := strings.Split(string(input), "\n")

	var xLow, yLow, xHigh, yHigh int
	var instr string
	var action func(int)

	re := regexp.MustCompile("(.*) (\\d+),(\\d+) through (\\d+),(\\d+)")
	for _, s := range strs {
		matches := re.FindStringSubmatch(s)
		instr = matches[1]
		xLow, _ = strconv.Atoi(matches[2])
		yLow, _ = strconv.Atoi(matches[3])
		xHigh, _ = strconv.Atoi(matches[4])
		yHigh, _ = strconv.Atoi(matches[5])

		switch instr {
		case "turn on":
			action = on
		case "turn off":
			action = off
		default:
			action = toggle
		}

		for x := xLow; x <= xHigh; x++ {
			for y := yLow; y <= yHigh; y++ {
				action(1000*x + y)
			}
		}
	}
}

func countLights(lights []int) int {
	count := 0
	for _, v := range lights {
		count += v
	}
	return count
}

func solution1(input []byte) {
	lights := make([]int, 1000 * 1000)

	on := func(i int) { lights[i] = 1 }
	off := func(i int) { lights[i] = 0 }
	toggle := func(i int) { lights[i] ^= 1 }

	simulate(input, on, off, toggle)
	fmt.Println("solution: ", countLights(lights))
}

func solution2(input []byte) {
	lights := make([]int, 1000 * 1000)

	on := func(i int) { lights[i]++ }
	off := func(i int) {
		if lights[i] > 0 {
			lights[i]--
		} else {
			lights[i] = 0
		}
	}
	toggle := func(i int) { lights[i] += 2 }

	simulate(input, on, off, toggle)
	fmt.Println("solution: ", countLights(lights))
}

func main() {
	filename := os.Args[2]
	part := os.Args[3]
	data, err := ioutil.ReadFile(filename)
	if err != nil {
		fmt.Println("error: ", err)
	}

	if part == "2" {
		solution2(data)
	} else {
		solution1(data)
	}
}
