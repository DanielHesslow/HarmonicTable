#pragma once 
char *note_name(int top, int bottom)
{
	if (top == 9 && bottom == 8)	return "Pythagorean major second";
	if (top == 9 && bottom == 7)	return "Septimal major third";
	if (top == 9 && bottom == 5)	return "Greater just minor seventh";
	if (top == 8 && bottom == 7)	return "Septimal major second";
	if (top == 8 && bottom == 5)	return "Just minor sixth";
	if (top == 81 && bottom == 80)	return "Syntonic comma";
	if (top == 81 && bottom == 50)	return "Acute minor sixth";
	if (top == 7 && bottom == 6)	return "Septimal minor third";
	if (top == 7 && bottom == 5)	return "Lesser septimal tritone";
	if (top == 7 && bottom == 4)	return "Septimal minor seventh";
	if (top == 6 && bottom == 5)	return "Just minor third";
	if (top == 63 && bottom == 32)	return "Sixty-third harmonic";
	if (top == 61 && bottom == 32)	return "Sixty-first harmonic";
	if (top == 5 && bottom == 4)	return "Just major third";
	if (top == 5 && bottom == 3)	return "Just major sixth";
	if (top == 59 && bottom == 32)	return "Fifty-ninth harmonic";
	if (top == 57 && bottom == 32)	return "Fifty-seventh harmonic";
	if (top == 56 && bottom == 55)	return "Ptolemy's enharmonic";
	if (top == 55 && bottom == 32)	return "Fifty-fifth harmonic";
	if (top == 53 && bottom == 32)	return "Fifty-third harmonic";
	if (top == 51 && bottom == 32)	return "Fifty-first harmonic";
	if (top == 50 && bottom == 49)	return "Septimal sixth tone";
	if (top == 50 && bottom == 27)	return "Grave major seventh";
	if (top == 4 && bottom == 3)	return "Perfect fourth";
	if (top == 49 && bottom == 48)	return "Septimal diesis";
	if (top == 49 && bottom == 32)	return "Forty-ninth harmonic";
	if (top == 48 && bottom == 25)	return "Classic diminished octave";
	if (top == 47 && bottom == 32)	return "Forty-seventh harmonic";
	if (top == 46 && bottom == 45)	return "Inferior quarter tone";
	if (top == 45 && bottom == 44)	return "Undecimal diesis";
	if (top == 45 && bottom == 32)	return "Just augmented fourth";
	if (top == 43 && bottom == 32)	return "Forty-third harmonic";
	if (top == 42 && bottom == 41)	return "Lesser 41-limit fifth tone";
	if (top == 41 && bottom == 40)	return "Greater 41-limit fifth tone";
	if (top == 41 && bottom == 32)	return "Forty-first harmonic";
	if (top == 40 && bottom == 39)	return "Tridecimal fifth tone";
	if (top == 40 && bottom == 27)	return "5-limit wolf fifth";
	if (top == 3 && bottom == 2)	return "Perfect fifth";
	if (top == 39 && bottom == 38)	return "Superior quarter-tone";
	if (top == 39 && bottom == 32)	return "Thirty-ninth harmonic";
	if (top == 38 && bottom == 37)	return "Lesser 37-limit quarter tone";
	if (top == 37 && bottom == 36)	return "Greater 37-limit quarter tone";
	if (top == 37 && bottom == 32)	return "Thirty-seventh harmonic";
	if (top == 36 && bottom == 35)	return "Septimal quarter tone";
	if (top == 36 && bottom == 25)	return "Just diminished fifth";
	if (top == 35 && bottom == 34)	return "ET quarter-tone approximation";
	if (top == 35 && bottom == 32)	return "Thirty-fifth harmonic";
	if (top == 35 && bottom == 18)	return "Septimal supermajor seventh";
	if (top == 34 && bottom == 33)	return "Greater 17-limit quarter tone";
	if (top == 33 && bottom == 32)	return "Thirty-third harmonic";
	if (top == 32 && bottom == 31)	return "Inferior quarter-tone";
	if (top == 32 && bottom == 27)	return "Pythagorean minor third semiditone";
	if (top == 32 && bottom == 25)	return "Just diminished fourth";
	if (top == 32 && bottom == 21)	return "21st subharmonic";
	if (top == 32 && bottom == 19)	return "19th subharmonic";
	if (top == 32 && bottom == 17)	return "17th subharmonic";
	if (top == 31 && bottom == 30)	return "Greater quarter-tone";
	if (top == 31 && bottom == 16)	return "Thirty-first harmonic";
	if (top == 30 && bottom == 29)	return "Lesser 29-limit quarter tone";
	if (top == 2 && bottom == 1)	return "Octave";
	if (top == 29 && bottom == 28)	return "Greater 29-limit quarter tone";
	if (top == 29 && bottom == 16)	return "Twenty-ninth harmonic";
	if (top == 28 && bottom == 27)	return "Septimal minor second";
	if (top == 27 && bottom == 26)	return "Chromatic diesis";
	if (top == 27 && bottom == 25)	return "Semitone maximus";
	if (top == 27 && bottom == 22)	return "Zalzal's wosta 12:11 X 9:8";
	if (top == 27 && bottom == 20)	return "5-limit wolf fourth";
	if (top == 27 && bottom == 16)	return "Pythagorean major sixth";
	if (top == 27 && bottom == 14)	return "Septimal major seventh";
	if (top == 26 && bottom == 25)	return "Tridecimal third tone";
	if (top == 25 && bottom == 24)	return "Just chromatic semitone";
	if (top == 25 && bottom == 21)	return "Quasi-equal-tempered minor third";
	if (top == 25 && bottom == 18)	return "Just augmented fourth";
	if (top == 25 && bottom == 16)	return "Just augmented fifth				";
	if (top == 24 && bottom == 23)	return "Lesser 23-limit semitone";
	if (top == 23 && bottom == 22)	return "Greater 23-limit semitone";
	if (top == 23 && bottom == 16)	return "Twenty-third harmonic";
	if (top == 22 && bottom == 21)	return "Hard semitone";
	if (top == 21 && bottom == 20)	return "Septimal chromatic semitone";
	if (top == 21 && bottom == 16)	return "Twenty-first harmonic";
	if (top == 20 && bottom == 19)	return "Novendecimal augmented unison";
	if (top == 20 && bottom == 11)	return "Lesser undecimal neutral seventh";
	if (top == 1 && bottom == 1)	return "Unison";
	if (top == 19 && bottom == 18)	return "Novendecimal minor second";
	if (top == 19 && bottom == 16)	return "19th harmonic";
	if (top == 18 && bottom == 17)	return "Just minor semitone";
	if (top == 18 && bottom == 11)	return "Undecimal neutral sixth";
	if (top == 17 && bottom == 16)	return "Minor diatonic semitone";
	if (top == 17 && bottom == 14)	return "Superminor third";
	if (top == 16 && bottom == 9)	return "Pythagorean minor seventh";
	if (top == 16 && bottom == 15)	return "Just minor second";
	if (top == 16 && bottom == 13)	return "Tridecimal neutral third";
	if (top == 16 && bottom == 11)	return "undecimal semi-diminished fifth";
	if (top == 15 && bottom == 8)	return "Just major seventh";
	if (top == 15 && bottom == 14)	return "Septimal diatonic semitone";
	if (top == 14 && bottom == 9)	return "Septimal minor sixth";
	if (top == 14 && bottom == 13)	return "Lesser tridecimal 2/3-tone";
	if (top == 14 && bottom == 11)	return "Undecimal diminished fourth";
	if (top == 13 && bottom == 8)	return "Tridecimal neutral sixth";
	if (top == 13 && bottom == 12)	return "Greater tridecimal 2/3-tone";
	if (top == 13 && bottom == 10)	return "Tridecimal major third";
	if (top == 12 && bottom == 7)	return "Septimal major sixth";
	if (top == 12 && bottom == 11)	return "Undecimal neutral second";
	if (top == 11 && bottom == 9)	return "Undecimal neutral third";
	if (top == 11 && bottom == 8)	return "eleventh harmonic";
	if (top == 11 && bottom == 7)	return "Undecimal minor sixth";
	if (top == 11 && bottom == 6)	return "Undecimal neutral seventh";
	if (top == 11 && bottom == 10)	return "Greater undecimal minor/major/neutral second";
	if (top == 10 && bottom == 9)	return "Small just whole tone";
	if (top == 10 && bottom == 7)	return "Greater septimal tritone";

	return "unnamed?";
}