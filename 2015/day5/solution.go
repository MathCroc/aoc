package main

import (
	"fmt"
	"os"
	"io/ioutil"
	"strings"
)

/*
It contains at least three vowels (aeiou only), like aei, xazegov, or aeiouaeiouaeiou.
It contains at least one letter that appears twice in a row, like xx, abcdde (dd), or aabbccdd (aa, bb, cc, or dd).
It does not contain the strings ab, cd, pq, or xy, even if they are part of one of the other requirements.
*/
func isNice(s string) bool {
	const vowels string = "aeiou"

	vowelCount := 0
	for _, c := range s {
		if strings.ContainsRune(vowels, c) {
			vowelCount++
		}
	}

	doubleLetter := false
	for i := range s[1:] {
		if s[i] == s[i + 1] {
			doubleLetter = true
			break
		}
	}

	containsNotAllowed := strings.Contains(s, "ab") ||
						  strings.Contains(s, "cd") ||
						  strings.Contains(s, "pq") ||
						  strings.Contains(s, "xy")

	return vowelCount >= 3 && doubleLetter && !containsNotAllowed
}

func solution1(input []byte) {
	strs := strings.Split(string(input), "\n")
	count := 0
	for _, s := range strs {
		if isNice(s) {
			count++
		}
	}

	fmt.Println("solution: ", count)
}

/*
It contains a pair of any two letters that appears at least twice in the string without overlapping, like xyxy (xy) or aabcdefgaa (aa), but not like aaa (aa, but it overlaps).
It contains at least one letter which repeats with exactly one letter between them, like xyx, abcdefeghi (efe), or even aaa.
*/
func isNice2(s string) bool {
	doublePair := false
	for i := range s[:len(s)-2] {
		if strings.Contains(s[i+2:], s[i:i+2]) {
			doublePair = true
			break
		}
	}

	doubleLetter := false
	for i := range s[:len(s)-2] {
		if s[i] == s[i + 2] {
			doubleLetter = true
			break
		}
	}

	return doublePair && doubleLetter
}

func solution2(input []byte) {
	strs := strings.Split(string(input), "\n")
	count := 0
	for _, s := range strs {
		if isNice2(s) {
			count++
		}
	}

	fmt.Println("solution: ", count)
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
