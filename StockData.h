#pragma once
#include <string>
class StockData {
protected:
	std::string symbol;
	float price;
	float change;
	float changePercent;
	float high;
	float low;
public:
	StockData(const std::string& symbol = "", float price = 0.0f, float change = 0.0f, float changePercent = 0.0f, float high = 0.0f, float low = 0.0f);


	virtual void display() const;
	std::string getSymbol() const;
	float getPrice() const;
	float getChange() const;
	float getChangePercent() const;
	float getHigh() const;
	float getLow() const;
	void setData(float p, float c, float cp, float h, float l);

};