package main

import (
	"fmt"
	"os"
	"io/ioutil"
	"strings"
	"regexp"
	"strconv"
)

func buildConnections(input []byte) map[string]string {
	strs := strings.Split(string(input), "\n")
	connect := make(map[string]string)
	for _, s := range strs {
		parts := strings.Split(s, " -> ")
		connect[parts[1]] = parts[0]
	}
	return connect
}

func getSignal(connect map[string]string, signals map[string]uint16, wire string) (signal uint16) {
	val, ok := signals[wire]
	if ok {
		signal = val
		return
	}

	defer func() { signals[wire] = signal }()

	// check if wire itself is a number
	re := regexp.MustCompile("^\\d+$")
	if re.MatchString(wire) {
		ret, _ := strconv.Atoi(wire)
		signal = uint16(ret)
		return
	}

	expr := connect[wire]

	// check if number
	if re.MatchString(expr) {
		ret, _ := strconv.Atoi(expr)
		signal = uint16(ret)
		return
	}

	// check if wire
	re = regexp.MustCompile("^[a-z]+$")
	if re.MatchString(expr) {
		signal = getSignal(connect, signals, expr)
		return
	}

	// check if unary expression
	re = regexp.MustCompile("^NOT ([a-z]+)$")
	matches := re.FindStringSubmatch(expr)
	if len(matches) > 1 && matches[0] == expr {
		signal = ^getSignal(connect, signals, matches[1])
		return
	}

	// check if binary expression
	re = regexp.MustCompile("^([a-z0-9]+) ([A-Z]+) ([a-z0-9]+)$")
	matches = re.FindStringSubmatch(expr)
	if len(matches) > 1 && matches[0] == expr {
		switch matches[2] {
		case "OR":
			signal = getSignal(connect, signals, matches[1]) | getSignal(connect, signals, matches[3])
		case "AND":
			signal = getSignal(connect, signals, matches[1]) & getSignal(connect, signals, matches[3])
		case "RSHIFT":
			signal = getSignal(connect, signals, matches[1]) >> getSignal(connect, signals, matches[3])
		case "LSHIFT":
			signal = getSignal(connect, signals, matches[1]) << getSignal(connect, signals, matches[3])
		default:
			fmt.Println("Not valid: ", matches[2])
			panic(1)
		}
	}

	return
}

func solution1(input []byte) {
	connect := buildConnections(input)
	signals := make(map[string]uint16)
	fmt.Println("solution: ", getSignal(connect, signals, "a"))
}

func solution2(input []byte) {
	connect := buildConnections(input)
	signals := make(map[string]uint16)
	initSignal := getSignal(connect, signals, "a")
	connect["b"] = strconv.Itoa(int(initSignal))

	signals = make(map[string]uint16)
	fmt.Println("solution: ", getSignal(connect, signals, "a"))
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
