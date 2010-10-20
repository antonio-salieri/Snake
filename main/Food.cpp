#include "Food.hpp"

#include "SentinelFood.hpp"

// GAMECONSTANT: food data
const Food::FoodInfo Food::ice(-2.5, Color24(0, 0, 255), -3000);
const Food::FoodInfo Food::celery(0.3, Color24(127, 255, 127), 250);
const Food::FoodInfo Food::normal(1.0, Color24(0, 255, 255), 1000);
const Food::FoodInfo Food::donut(3.0, Color24(200, 0, 0), 4000);

Food::Food(const SentinelFood& prototype, const FoodInfo& foodInfo) :
	WorldObject(food)
{
	pointsGiven = 0;
	eaten = false;
	bounds = prototype.GetBounds();
	color = foodInfo.color;
	calories = foodInfo.calories;
}

void Food::SnakeCollisionHandler()
{
	eaten = true;
}

void Food::CollisionHandler(const WorldObject& obj)
{
	if(obj.GetObjectType() == snake)
		SnakeCollisionHandler();
}

bool Food::IsEaten() const
{
	return eaten;
}

double Food::GetCalories() const
{
	return calories;
}

int Food::GetPoints() const
{
	return pointsGiven;
}
