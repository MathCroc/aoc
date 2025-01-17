use std::collections::HashSet;
use std::env;
use std::fs;
use std::str;

fn main() {
    let args: Vec<String> = env::args().collect();

    let alt_input = if args.len() > 2 {
        let path = &args[2];
        fs::read_to_string(path).expect("couldn't read input")
    } else {
        String::from("")
    };

    let input = if args.len() > 2 {
        alt_input.as_str()
    } else {
        include_str!("../data/input.txt")
    };

    let start = std::time::Instant::now();

    match args[1].parse() {
        Ok(1) => part1(input),
        Ok(2) => part2(input),
        Ok(x) => panic!("invalid part number {x}"),
        Err(y) => panic!("{y}"),
    }

    println!("Processing time: {}ms", start.elapsed().as_millis());
}

fn turn(dir: [i32; 2], label: u8) -> [i32; 2] {
    match label {
        b'R' => [dir[1], -dir[0]],
        b'L' => [-dir[1], dir[0]],
        _ => unreachable!(),
    }
}

fn steps(s: &[u8]) -> i32 {
    let tmp = str::from_utf8(s).unwrap();
    return i32::from_str_radix(tmp, 10).unwrap();
}

fn part1(s: &str) {
    let mut pos = [0; 2];
    let mut dir = [0, 1];

    s.split(", ").map(|s| s.as_bytes()).for_each(|s| {
        dir = turn(dir, s[0]);
        let d = steps(&s[1..]);

        pos[0] += dir[0] * d;
        pos[1] += dir[1] * d;
    });

    let distance = pos[0].abs() + pos[1].abs();
    println!("Part 1: {distance}");
}

fn part2(input: &str) {
    let mut pos: [i32; 2] = [0; 2];
    let mut dir = [0, 1];
    let mut visited = HashSet::new();

    visited.insert(pos);

    for s in input.split(", ").map(|x| x.as_bytes()) {
        dir = turn(dir, s[0]);
        let d = steps(&s[1..]);

        for _ in 0..d {
            pos[0] += dir[0];
            pos[1] += dir[1];
            if visited.contains(&pos) {
                let distance = pos[0].abs() + pos[1].abs();
                println!("Part 2: {distance}");
                return;
            }
            visited.insert(pos);
        }
    }

    unreachable!();
}
