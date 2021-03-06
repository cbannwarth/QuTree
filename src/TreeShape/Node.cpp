#include "TreeShape/Node.h"

Node::Node()
	: nTotalNodes_(1), nLeaves_(0),
	  nNodes_(1), nodeType_(1), bottomLayer_(false),
	  nextNodeNum_(0), nextNodeNumFortran_(0), address_(-100), up_(nullptr) {
}

// Copy constructor
Node::Node(const Node& node)
	: nTotalNodes_(node.nTotalNodes_), nNodes_(node.nNodes_),
	  nLeaves_(node.nLeaves_), tensorDim_(node.tensorDim_),
	  up_(node.up_),
	  nextNodeNum_(node.nextNodeNum_),
	  nextNodeNumFortran_(node.nextNodeNumFortran_),
	  position_(node.position_),
	  address_(node.address_), nodeType_(node.nodeType_),
	  bottomLayer_(node.bottomLayer_) {
	if (isBottomlayer()) {
		down_.emplace_back(make_unique<Leaf>(node.getLeaf()));
		getLeaf().setParent(this);
	} else {
		for (size_t i = 0; i < node.nChildren(); i++) {
			down_.emplace_back(make_unique<Node>(node.child(i)));
			child(i).setParent(this);
		}
	}
}

// move constructor
Node::Node(Node&& node) noexcept
	: nTotalNodes_(node.nTotalNodes_), nNodes_(node.nNodes_),
	  nLeaves_(node.nLeaves_), tensorDim_(node.tensorDim_),
	  up_(node.up_),
	  nextNodeNum_(node.nextNodeNum_), position_(node.position_),
	  address_(node.address_), nodeType_(node.nodeType_),
	  bottomLayer_(node.bottomLayer_), nextNodeNumFortran_(node.nextNodeNumFortran_) {
	// Set upwards connectivity of children
	down_ = move(node.down_);
	if (node.isBottomlayer()) {
		getLeaf().setParent(this);
	} else {
		for (size_t i = 0; i < nChildren(); i++) {
			child(i).setParent(this);
		}
	}
}

Node& Node::operator=(const Node& old) {
	Node node(old);
	*this = move(node);
	return *this;
}

Node& Node::operator=(Node&& old) noexcept {
	if (this == &old) {
		return *this;
	}
	nTotalNodes_ = old.nTotalNodes_;
	nNodes_ = old.nNodes_;
	nLeaves_ = old.nLeaves_;
	tensorDim_ = old.tensorDim_;
	up_ = old.up_;
	nextNodeNum_ = old.nextNodeNum_;
	position_ = old.position_;
	address_ = old.address_;
	nodeType_ = old.nodeType_;
	bottomLayer_ = old.bottomLayer_;
	down_ = move(old.down_);
	// Set upwards connectivity of children
	if (isBottomlayer()) {
		getLeaf().setParent(this);
	} else {
		for (size_t i = 0; i < nChildren(); i++) {
			child(i).setParent(this);
		}
	}

	return *this;
}

Node::Node(const Leaf& phys, size_t ntensor)
	: Node() {
	// This constructor initializes a bottomLayer_ GetNode from a
	// Leaf
//	down_.emplace_back(unique_ptr<Leaf>(new Leaf(phys)));
	down_.emplace_back(make_unique<Leaf>(phys));
	nTotalNodes_++;
	nLeaves_++;
	bottomLayer_ = true;
	nextNodeNum_ = 0;
	nextNodeNumFortran_ = 0;

	// Set connectivity of linearizedLeaves_ node
	Leaf& phy = getLeaf();
	phy.setParent(this);

	// Build the TensorDim
	tensorDim_ = TensorShape({phys.Dim(), ntensor});
}

void Node::Initialize(istream& file, Node *up,
	const NodePosition& position) {
	up_ = up;
	position_ = position;

	// Variables needed to read input file
	int nstates, f;
	vector<size_t> dim;

	// read input file
	file >> nstates;
	assert(nstates > 0);
	file >> f;
	f = -f;
	assert(f > 0);

	for (int i = 0; i < f; i++) {
		// check wether physical or logical coord
		// and reset stream to old position_
		int newstate, newf;
		auto mark = file.tellg();
		file >> newstate;
		assert(newstate > 0);
		file >> newf;
		file.seekg(mark);

		dim.push_back(newstate);

		// generate position_ for the new node
		NodePosition newposition = position_ * i;

		if (newf < 0) {
			// add logical node
			down_.emplace_back(unique_ptr<Node>
				(new Node(file, this, newposition)));
			nTotalNodes_ += (*down_[i]).nTotalNodes();
			nNodes_ += (*down_[i]).nNodes();
			nLeaves_ += (*down_[i]).nLeaves();
		} else {
			// add physical node
			down_.emplace_back(unique_ptr<Leaf>
				(new Leaf(file, this, newposition)));
			// A physical node was added, so increase the number of nodes by 1
			nTotalNodes_++;
			nLeaves_++;
			// If this layer_ holds a Physical Coordinate it is a bottom layer_
			bottomLayer_ = true;
		}
	}

	// create a TensorDim after dimensions were read
	dim.push_back(nstates);
	tensorDim_ = TensorShape(dim);

	nextNodeNum_ = down_.size() - 1;
}

Node::Node(istream& file, Node *up,
	const NodePosition& position)
	: up_(up), nTotalNodes_(1),
	  nLeaves_(0), nNodes_(1), position_(position),
	  nodeType_(1), bottomLayer_(false), nextNodeNum_(0),
	  nextNodeNumFortran_(0), address_(-100) {
	// Call Initialize
	Initialize(file, up, position_);
}

void Node::info(ostream& os) const {
	position_.info(os);
//	tensorDim_.print(os);
}

void Node::Write(ostream& os) const {
	const TensorShape& tdim = shape();
	for (size_t l = 0; l < position_.Layer(); l++) { os << "\t"; }
	os << tdim.lastDimension() << "\t-" << nChildren() << "\n";
	for (size_t i = 0; i < nChildren(); i++) {
		down_[i]->Write(os);
	}
}

void Node::push_back(const Node& node) {
	down_.emplace_back(std::make_unique<Node>(node));
	down_.back()->setParent(this);
	Updatennodes();
}

bool Node::isToplayer() const {
	return (up_ == nullptr);
}

Leaf& Node::getLeaf() {
	assert(bottomLayer_);
	AbstractNode *node = down_[0].get();
	return (Leaf&) (*node);
}

const Leaf& Node::getLeaf() const {
	assert(bottomLayer_);
	AbstractNode *node = down_[0].get();
	return (Leaf&) (*node);
}

const Node& Node::child(size_t i) const {
	assert(i < down_.size());
	assert(!bottomLayer_);
	return (Node&) *down_[i];
}

Node& Node::child(size_t i) {
	assert(i < down_.size());
	assert(!bottomLayer_);
	return (Node&) *down_[i];
}

Node& Node::parent() {
	assert(up_ != nullptr);
	return (Node&) *up_;
}

const Node& Node::parent() const {
	assert(up_ != nullptr);
	return (Node&) *up_;
}

AbstractNode *Node::nextNode() {

	AbstractNode *result;
	if (nextNodeNum_ >= 0) {
		result = down_[nextNodeNum_].get()->nextNode();
		if (result == down_[nextNodeNum_].get()) {
			nextNodeNum_--;
		}
	} else {
		nextNodeNum_ = down_.size() - 1;
		result = this;
	}

	return result;
}

AbstractNode *Node::nextNodeManthe() {

	AbstractNode *result;
	if (nextNodeNumFortran_ < down_.size()) {
		result = down_[nextNodeNumFortran_].get()->nextNodeManthe();
		if (result == down_[nextNodeNumFortran_].get()) {
			++nextNodeNumFortran_;
		}
	} else {
		nextNodeNumFortran_ = 0;
		result = this;
	}

	return result;
}

unique_ptr<AbstractNode> Node::DownUnique(size_t i) {
	assert(i < down_.size());
	return move(down_[i]);
}

void Node::expandChild(size_t i) {
	assert(!isBottomlayer());
	assert(i < down_.size());

	// Make a new "down_new" by adding all old children
	// except the expanded node and adding the expanded node's children
	// at the right place

	vector<unique_ptr<AbstractNode>> down_new;
	// move nodes until expanded node to down_new 
	for (size_t j = 0; j < i; j++) {
		down_new.push_back(move(down_[j]));
	}

	// append the children of the expanded node to down_new
	Node& child = this->child(i);
	assert(!child.isBottomlayer());
	size_t nchildren = child.nChildren();
	for (size_t j = 0; j < nchildren; j++) {
		// Update parent-ptr of new children
		Node& subnode = child.child(j);
		subnode.setParent(this);
		// Update subnodes positionindices
		int child_nr = down_new.size();
		subnode.UpdatePosition(position_ * child_nr);

		// Save the unique_ptr to the new vector
		unique_ptr<AbstractNode> subchild = child.DownUnique(j);
		down_new.push_back(move(subchild));
	}

	// move the rest of the old children to down_new
	for (size_t j = i + 1; j < nChildren(); j++) {
		down_new.push_back(move(down_[j]));
	}

	// move down_new to down_
	down_ = move(down_new);

	// Adjust TensorDim
	UpdateTDim();

	// Update position
	UpdatePosition(position_);

	// update nTotalNodes_: All nTotalNodes_ of the nodes above change
	// so go to the topnode and update from there downwards
	Node& topnode = TopNode();
	topnode.Updatennodes();

	// Set the nextNodeNum_ for a correct nextNode() sweep that starts
	// at the last_ node
	nextNodeNum_ = down_.size() - 1;
}

void Node::update(const NodePosition& p) {
	// @TODO: Should reset state_ and Update(connectivity) be in separate routines?
	ResetCounters();
	UpdatePosition(p);
	Updatennodes();
	UpdateTDim();
}

void Node::UpdateTDim() {
/*	size_t ntensor = 0;
	if (IsToplayer()) {
		// If the node is a Toplayer, ntensor stays the same
		ntensor = tensorDim_.getntensor();
	} else {
		// If this node is not a toplayer-node, ntensor is given by the parents 
		// active_ size
		Node& parent = parent();
		// @TODO: This looks wrong - check again. Doesnt it have to be active_(k)?
		ntensor = parent.shape().LastActive();
	}*/

	// Get the dimensions of the children by requesting their ntensors
	vector<size_t> dim_new;
	if (isBottomlayer()) {
		const Leaf& phys = getLeaf();
		dim_new.push_back(phys.Dim());
	} else {
		for (int i = 0; i < nChildren(); i++) {
			const Node& child = this->child(i);
			const TensorShape& tdimchild = child.shape();
			dim_new.push_back(tdimchild.lastDimension());
		}
	}

	// Create a new TensorDim from the dim_-vector and ntensor
	dim_new.push_back(tensorDim_.lastDimension());
	tensorDim_ = TensorShape(dim_new);
}

void Node::UpdatePosition(const NodePosition& p) {
	// Save the new position_
	position_ = p;

	// Update positions of children
	for (size_t i = 0; i < down_.size(); i++) {
		NodePosition subp = p * i;
		if (isBottomlayer()) {
			Leaf& phy = getLeaf();
			phy.UpdatePosition(subp);
		} else {
			Node& child = this->child(i);
			child.UpdatePosition(subp);
		}
	}
}

void Node::ResetCounters() {

	nextNodeNum_ = down_.size() - 1;
	nextNodeNumFortran_ = 0;
	if (!isBottomlayer()) {
		for (size_t k = 0; k < nChildren(); ++k) {
			child(k).ResetCounters();
		}
	}
}

void Node::Updatennodes() {
	if (isBottomlayer()) {
		nTotalNodes_ = 2;
		nNodes_ = 1;
		nLeaves_ = 1;
	} else {
		nTotalNodes_ = 1;
		nNodes_ = 1;
		nLeaves_ = 0;
		for (size_t k = 0; k < down_.size(); k++) {
			Node& child = this->child(k);
			child.Updatennodes();
			nTotalNodes_ += child.nTotalNodes();
			nNodes_ += child.nNodes();
			nLeaves_ += child.nLeaves();
		}
	}
}

Node& Node::TopNode() {
	// Returns the topnode of the tree
	if (isToplayer()) {
		return (*this);
	} else {
		return parent().TopNode();
	}
}

void Node::Replace(Node& new_child, size_t idx) {
	assert(idx < down_.size());
	down_[idx] = unique_ptr<Node>(new Node(new_child));
}



