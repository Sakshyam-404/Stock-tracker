#include "StockData.h"
#include <iostream>
StockData::StockData(const std::string& symbol, float price, float change, float changePercent, float high, float low)
	: symbol(symbol), price(price), change(change), changePercent(changePercent), high(high), low(low) {
}
void StockData::display() const {
	std::cout << "Symbol: " << symbol << "\n"
		<< "Price: " << price << "\n"
		<< "Change: " << change << "\n"
		<< "Change Percent: " << changePercent << "%\n"
		<< "High: " << high << "\n"
		<< "Low: " << low << std::endl;
}
std::string StockData::getSymbol() const {
	return symbol;
}
float StockData::getPrice() const {
	return price;
}
float StockData::getChange() const {
	return change;
}
float StockData::getChangePercent() const {
	return changePercent;
}
float StockData::getHigh() const {
	return high;
}
float StockData::getLow() const {
	return low;
}
void StockData::setData(float p, float c, float cp, float h, float l) {
	price = p;
	change = c;
	changePercent = cp;
	high = h;
	low = l;
}
