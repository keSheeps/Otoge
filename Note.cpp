# include "Note.hpp"

Note::Note(double pos, noteType type, double left, double right, MPMChangeList mpmCngList) {
	this->timing = mpmCngList.getSec(pos);
	this->type = type;
	this->left = left;
	this->right = right;
}

std::tuple<double, double, double> Note::getPosition() {
	return std::tuple<double,double,double>(timing,left,right);
}

void Note::ApplyOffset(double offset) {
	timing += offset;
}

noteType Note::getType(void) {
	return type;
}
