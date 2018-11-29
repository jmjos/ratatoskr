////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Jan Moritz Joseph
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
/////////////////////////////////////////////////////////////////////////////////
#include <utils/Statistics.h>

Statistics::Statistics(std::string name) {
	sampleSum = 0.0f;
	sampleSize = 0;
	sampleMin = std::numeric_limits<float>::max();
	sampleMax = std::numeric_limits<float>::min();
	this->name = name;
}

float Statistics::average() {
	return (sampleSum / (float) sampleSize);
}
float Statistics::min() {
	return sampleMin;
}
float Statistics::max() {
	return sampleMax;
}
float Statistics::sum() {
	return sampleSum;
}
long Statistics::samplesize() {
	return sampleSize;
}
bool Statistics::sample(float sample) {
	sampleSum += sample;
	sampleSize++;
	if (sampleMin > sample)
		sampleMin = sample;
	if (sampleMax > sample)
		sampleMax = sample;
}
void Statistics::report(ostream& stream) {
	float avg = average();
	stream << boost::format("%s was on average %0.3f") % name % avg;
}
