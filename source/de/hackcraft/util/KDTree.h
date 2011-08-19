/*
 * File:   KDTree.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * KD-Tree for clustering k-dimensional data vectors
 * and finding the nearest stored data vectors (or associated data)
 * for a given sample or for a given sample interval.
 *
 * The current implementation is for float-array vectors but you
 * may easily replace it with another typed array.
 *
 * Created on August 29, 2008, 12:00 PM
 */

#ifndef _KDTREE_H
#define	_KDTREE_H

#include <cmath>
#include <list>
#include <functional>
#include <cassert>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

template <class VALUE>
class KDTree {
public:
    struct KDNode;
    int kdim;
    KDNode* root;

public:

    /**
     * Constructs an immutable kd-tree of the given dimensionality (kdim)
     * for the given dataset (and corresponding userset).
     * Datavectors must be of dimension kdim.
     */
    KDTree(unsigned int kdim, std::list<float*>* dataset, std::list<VALUE>* userset = NULL) {
        if (kdim < 1) throw "kdim has to be 1 at least.";
        if (dataset == NULL) throw "dataset may not be NULL.";
        this->kdim = kdim;
        root = build(kdim, 0, 0, dataset, userset);
        if (root == NULL) throw "kd-build returned NULL.";
    }

    ~KDTree() {
        //printf("Before delete #nodes == %i\n", KDNode::sCount);
        delete root;
        //printf("After delete #nodes == %i\n", KDNode::sCount);
    }

    /**
     * Returns amount of (not necessarily unique) stored datavectors.
     */
    int size() {
        return root->size();
    }

    /**
     * Returns the number of dimensions a stored datavector has/must have.
     */
    int dim() {
        return kdim;
    }

    /**
     * Returns true if the tree contains no data (as of now never).
     */
    bool empty() {
        return false;
    }

    /**
     * Returns a list of similar datavectors to the given datavector
     * note that when the list contains more than one datavector then
     * they are all equal (at least within an epsilon).
     * The returned list is freshly allocated (new'ed).
     * To delete is caller responsibility.
     */
    std::list<float*>* findDataset(float* datavec) {
        std::list<float*>* original = root->findDataset(kdim, 0, datavec);
        return new std::list<float*>(original->begin(), original->end());
    }

    /**
     * Similar to findDataset(..) but finds datavectors near to
     * the given interval.
     * The returned list is freshly allocated (new'ed).
     * To delete is caller responsibility.
     */
    std::list<float*>* findDatasetInterval(float* min_datavec, float* max_datavec) {
        return root->findDatasetInterval(kdim, 0, min_datavec, max_datavec);
    }

    /**
     * Returns a list of associated userdata for similar datavectors
     * to the given datavector note that when the list contains more
     * than one userdata then the found datavectors where all equal
     * (at least within an epsilon).
     * The returned list is freshly allocated (new'ed).
     * To delete is caller responsibility.
     */
    std::list<VALUE>* findUserset(float* datavec) {
        std::list<VALUE>* original = root->findUserset(kdim, 0, datavec);
        return new std::list<VALUE>(original->begin(), original->end());
    }

    /**
     * Similar to findUserset(..) but finds userdata for datavectors
     * near to the given interval.
     * The returned list is freshly allocated (new'ed).
     * To delete is caller responsibility.
     */
    std::list<VALUE>* findUsersetInterval(float* min_datavec, float* max_datavec) {
        return root->findUsersetInterval(kdim, 0, min_datavec, max_datavec);
    }

//private:
public:

    /**
     * Constructs a KD-Tree of the given dimensionality (kdim) for the
     * given dataset. Note that the pointers in the dataset are stored
     * in the tree. The algorithm identifies equal dimensions and delivers
     * optimized branches (throw/catch is used to return leafes upwards to
     * the branch where the last difference in data was).
     *
     * kdim : Dimensionality of datavectors.
     * curlvl : Current level, should be 0 initialy.
     * equaldims : Equal dimensions, should be 0 initialy.
     * dataset: List of datavectors of dimension kdim to cluster.
     * userset: Associated userdata/objects for each datavector
     * (or userset == NULL <=> no userdata at all).
     *
     * The function is roughly divided into:
     * - Terminal case handling for leaves
     * - Partitioning data into two sets left and right
     * - Recursion for left sub-tree/partition
     * - Recursion for right sub-tree/partition
     */
    KDNode * build(unsigned int kdim, unsigned int curlvl, unsigned int equaldims, std::list<float*>* dataset, std::list<VALUE>* userset = NULL) {
        // Current dimension for clustering
        unsigned int index = curlvl % kdim;

        //printf("buildKDTree: kdim %i  curlvl %i  equaldims %i  dataset %i\n", kdim, curlvl, equaldims, dataset->size());

        // Only one Vector left or all dimensions equal?
        if (dataset->size() == 1 || equaldims == kdim) {
            /*
            if (equaldims == kdim) {
                printf("equalsize %i, lvl %i\n", dataset->size(), curlvl);
            }
             */
            //printf("buildKDTree: leaf\n");
            float* v = dataset->front();
            KDNode* node = new KDNode();
            node->left = NULL;
            node->right = NULL;
            node->comparevalue = v[index];
            node->dataset = dataset;
            node->userset = userset;
            // Pathological case (no difference in data at all):
            if (curlvl == 0) {
                node->dataset = new std::list<float*>;
                node->dataset->insert(node->dataset->begin(), dataset->begin(), dataset->end());
                if (userset != NULL) {
                    node->userset = new std::list<VALUE>;
                    node->userset->insert(node->userset->begin(), userset->begin(), userset->end());
                }
            }
            if (equaldims == kdim) {
                //printf("discovered equal dimensions: %i\n", dataset->size());
                throw node; // All equal Nodes special return
            }
            //printf("adding single data leaf\n");
            return node;
        }

        // Find Min and Max value in current dimension for binning:
        float min = +10000000.0f; // posinf better
        float max = -10000000.0f; // neginf better
        for (std::list<float*>::iterator i = dataset->begin(); i != dataset->end(); i++) {
            float value = (*i)[index];
            if (value < min) min = value;
            if (value > max) max = value;
        }

        // Inverse of difference between minimum and maximum value.
        // 1.0f if min == max.
        float extends_inverse = (min < max) ? (1.0f / (max - min)) : (1.0f);
        //printf("  min- and max-value for binning: %f <= %f, scale %f\n", min, max, extends_inverse);

        // Allocate Bins: 2 + floor( log2(number of datavectors) ) bins.
        //   log2(n) == ln(n) / ln(2)
        //   1.0/ln(2) == 1.443
        unsigned int nbins = 2 + (int) (log(dataset->size()) * 1.443);
        unsigned int* bins = new unsigned int[nbins];
        memset(bins, 0, sizeof (unsigned int) * nbins);
        //printf("  #Bins: %i\n", nbins );

        // A datavector may fall into bin 0...nbins-1
        float factor = (nbins - 1) * extends_inverse;
        bool randsamp = dataset->size() > 16 ? true : false;
        int cnt = 0;
        for (std::list<float*>::iterator i = dataset->begin(); i != dataset->end(); i++) {
            cnt++;
            if (randsamp && ((cnt&7) == 7)) continue;
            float value = (*i)[index];
            if (value != value) throw "Not-A-Number discovered while building kd-tree!";
            unsigned int bin = (unsigned int) ((value - min) * factor);
            //printf("  value: %f  bin: %i\n", value, bin);
            bins[bin]++;
        }

        // Debug print Bins
        //printf("  "); for (int i = 0; i < nbins; i++) {printf("%i ", bins[i]);}printf("\n");

        // Surface Area Heuristic - ie. left/right about half/half
        int pre = 0;
        int count = 0;
        int half = dataset->size() / 2;
        unsigned int split = 0;
        for (split = 0; split < nbins; split++) {
            count += bins[split];
            if (count > half && pre > 0) {
                break;
            }
            pre = count;
        }
        float splitvalue = split / factor + min;
        //printf("  Count: %i  Splitpos %i  Splitvalue: %f\n", count, split, splitvalue);

        // Free Bins
        delete bins;

        // Splitting - separate dataset into two datasets left and right.

        typename std::list<VALUE>::iterator j;
        std::list<VALUE>* left_userset = NULL;
        std::list<VALUE>* right_userset = NULL;
        if (userset != NULL) {
            left_userset = new std::list<VALUE>;
            right_userset = new std::list<VALUE>;
            j = userset->begin();
        }

        std::list<float*>* left_list = new std::list<float*>;
        std::list<float*>* right_list = new std::list<float*>;
        for (std::list<float*>::iterator i = dataset->begin(); i != dataset->end(); i++) {
            float value = (*i)[index];
            if (value < splitvalue) {
                left_list->push_back((*i));
                if (left_userset != NULL) left_userset->push_back((*j));
            } else {
                right_list->push_back((*i));
                if (right_userset != NULL) right_userset->push_back((*j));
            }
            if (userset != NULL) j++;
        }
        //printf("<<< %i | %i >>>\n", left->size(), right->size());

        // Free the given dataset if it was not the original/initial set.
        if (curlvl > 0) {
            delete dataset;
            if (userset != NULL) {
                delete userset;
            }
        }

        // Create Sub-KDTree
        KDNode* node = new KDNode();
        node->comparevalue = splitvalue;
        node->dataset = NULL;
        node->userset = NULL;

        // Isn't there any difference in the current dimension?
        if (left_list->size() == 0 || right_list->size() == 0) equaldims++;
        else equaldims = 0;

        /*
        if (left_list->size() == 0 || right_list->size() == 0) {
            if (curlvl > 3) {
                printf("lvl %i, size %i, [%.2f - %.2f], splitvalue %.2f, eqdim %i, nbins %i, factor %.2f, split %i\n", curlvl, left_list->size()+right_list->size(), min, max, splitvalue, equaldims, nbins, factor, split);
                for (int i = 0; i < nbins; i++) {
                    printf("%i ", bins[i]);
                }
                printf("\n");
            }
        }
        */

        if (left_list->size() > 0) {
            //printf("<<<\n");
            try {
                node->left = build(kdim, curlvl + 1, equaldims, left_list, left_userset);
            } catch (KDNode* nd) {
                //printf("catching from left\n");
                // All dimensions are equal, did it start here?
                if (equaldims == 0) {
                    node->left = nd;
                } else {
                    delete node;
                    // Throw-return the indifferent left node.
                    //assert(right_list->size() == 0);
                    throw nd;
                }
            }
        } else {
            // Free the left set: It wasn't used.
            delete left_list;
            delete left_userset;
            node->left = NULL;
        }

        if (right_list->size() > 0) {
            //printf(">>>\n");
            try {
                node->right = build(kdim, curlvl + 1, equaldims, right_list, right_userset);
            } catch (KDNode* nd) {
                //printf("catching from right\n");
                // All dimensions are equal, did it start here?
                if (equaldims == 0) {
                    node->right = nd;
                } else {
                    delete node;
                    // Throw-return the indifferent right node.
                    //assert(left_list->size() == 0);
                    throw nd;
                }
            }
        } else {
            // Free the right set: It wasn't used.
            delete right_list;
            delete right_userset;
            node->right = NULL;
        }

        return node;
    } // kdtree::build()

    /**
     * KDNode represents a Single KD-Tree node/leaf or
     * a complete tree/subtree as well. This structure is for use
     * in kdtree class only!
     */
    struct KDNode {
        /// Value which splits the data into two halfs along current dimension.
        float comparevalue;
        /// Left subtree contains only datavectors v where v[dim] < comparevalue;
        KDNode *left;
        /// Right subtree contains only datavectors v where v[dim] >= comparevalue;
        KDNode *right;
        /// If this node is a leaf then dataset stores same datavectors (or one).
        std::list<float*>* dataset;
        /// If this node is a leaf then userset stores associated userdata.
        std::list<VALUE>* userset;

        /**
         * Dummy ordering/compare function for merge.
         */
        template <typename T>
                struct no_specific_order : std::binary_function<T, T, bool> {
            /// Always return true - ie x is always less than y.

            bool operator()(const T& x, const T & y) const {
                return true;
            };
        };

        KDNode() {
            comparevalue = 0;
            left = NULL;
            right = NULL;
            dataset = NULL;
            userset = NULL;
        }

        ~KDNode() {
            if (dataset) delete dataset;
            if (userset) delete userset;
            if (left) delete left;
            if (right) delete right;
        }

        /**
         * Print size of KD-Tree leaves.
         */
        void printLeavesize() {
            if (this->left == NULL && this->right == NULL) {
                printf("%i\n", this->dataset->size());
            }
            if (this->left != NULL) this->left->printLeavesize();
            if (this->right != NULL) this->right->printLeavesize();
        } // KDNode->printLeavesize()

        /**
         * Returns amount of (not necessarily unique) stored datavectors.
         */
        int size() {
            if (this->left == NULL && this->right == NULL) return this->dataset->size();
            int sum = 0;
            if (this->left != NULL) sum += this->left->size();
            if (this->right != NULL) sum += this->right->size();
            return sum;
        } // KDNode->size()

        /**
         * Search for data vectors near to the given data vector interval
         * (ie. searching for data vectors matching multidimensional Intervals).
         * Note that the result may contain more data than just the data vectors
         * within the interval (false positives, or fuzziness).
         * You should delete returned sets!! Caller responsibility!
         */
        std::list<float*>* findDatasetInterval(unsigned int kdim, unsigned int curlvl, float* min_datavec, float* max_datavec) {
            const float epsilon = 0.00001f;
            //printf("findIntervalInKDTree()\n");

            // Terminal case? If so then create and return a leaf.
            if (this->left == NULL && this->right == NULL) {
                std::list<float*>* ret = new std::list<float*>;
                ret->insert(ret->begin(), this->dataset->begin(), this->dataset->end());
                //printf("leaf size %i\n", ret->size());
                return ret;
            }

            // Current dimension for clustering
            unsigned int index = curlvl % kdim;

            // Interval fully left?
            if (max_datavec[index] < this->comparevalue) {
                //printf("left\n");
                // If the data for the dimension is the same then only
                // either left or right will contain the data.
                if (this->left != NULL) return this->left->findDatasetInterval(kdim, curlvl + 1, min_datavec, max_datavec);
                else return this->right->findDatasetInterval(kdim, curlvl + 1, min_datavec, max_datavec);

                // Interval fully right?
            } else if (min_datavec[index] >= this->comparevalue) {
                //printf("right\n");
                // If the data for the dimension is the same then only
                // either left or right will contain the data.
                if (this->right != NULL) return this->right->findDatasetInterval(kdim, curlvl + 1, min_datavec, max_datavec);
                else return this->left->findDatasetInterval(kdim, curlvl + 1, min_datavec, max_datavec);

                // Else: Interval extends over left and right.
            } else { // Split interval [min,max] into two: [min,...] [...,max]
                //printf("split\n");
                //printf("splitting at level %i for dimension %i\n", curlvl, index);
                std::list<float*>* ret = new std::list<float*>;

                // If there is data on the left side of the tree then collect.
                if (this->left != NULL) {
                    // take left half-interval.
                    float* left_min = new float[kdim];
                    float* left_max = new float[kdim];
                    memcpy(left_min, min_datavec, sizeof (float) * kdim);
                    memcpy(left_max, max_datavec, sizeof (float) * kdim);
                    left_max[index] = this->comparevalue - epsilon; // sure less than.
                    std::list<float*>* left_list = this->left->findDatasetInterval(kdim, curlvl + 1, left_min, left_max);
                    delete left_min;
                    delete left_max;
                    ret->merge(*left_list, no_specific_order<float*>());
                    delete left_list;
                }

                // If there is data on the right side of the tree then collect.
                if (this->right != NULL) {
                    // take right half-interval.
                    float* right_min = new float[kdim];
                    float* right_max = new float[kdim];
                    memcpy(right_min, min_datavec, sizeof (float) * kdim);
                    memcpy(right_max, max_datavec, sizeof (float) * kdim);
                    right_min[index] = this->comparevalue; // sure greater or equal than.
                    std::list<float*>* right_list = this->right->findDatasetInterval(kdim, curlvl + 1, right_min, right_max);
                    delete right_min;
                    delete right_max;
                    ret->merge(*right_list, no_specific_order<float*>());
                    delete right_list;
                }

                //printf("joining at level %i for dimension %i\n", curlvl, index);
                //printf("merging two lists: %i + %i = ", left->size(), right->size());
                //printf(" %i\n", ret->size());

                return ret;
            } // split interval.
        } // KDNode->findIntervalInKDTree()

        /**
         * Search for data vectors near to the given data vector interval
         * (ie. searching for data vectors matching multidimensional Intervals) and
         * returns their associated userdata (objects related to those vectors).
         * Note that the result may contain more data than just
         * within the interval (false positives, or fuzziness).
         * You should delete returned sets!! Caller responsibility!
         */
        std::list<VALUE>* findUsersetInterval(unsigned int kdim, unsigned int curlvl, float* min_datavec, float* max_datavec) {
            const float epsilon = 0.00001f;
            //printf("findIntervalInKDTree()\n");

            // Terminal case? If so then create and return a leaf.
            if (this->left == NULL && this->right == NULL) {
                std::list<VALUE>* ret = new std::list<VALUE>;
                ret->insert(ret->begin(), this->userset->begin(), this->userset->end());
                //printf("leaf size %i\n", ret->size());
                return ret;
            }

            // Current dimension for clustering
            unsigned int index = curlvl % kdim;

            // Interval fully left?
            if (max_datavec[index] < this->comparevalue) {
                //printf("left\n");
                // If the data for the dimension is the same then only
                // either left or right will contain the data.
                if (this->left != NULL) return this->left->findUsersetInterval(kdim, curlvl + 1, min_datavec, max_datavec);
                else return this->right->findUsersetInterval(kdim, curlvl + 1, min_datavec, max_datavec);

                // Interval fully right?
            } else if (min_datavec[index] >= this->comparevalue) {
                //printf("right\n");
                // If the data for the dimension is the same then only
                // either left or right will contain the data.
                if (this->right != NULL) return this->right->findUsersetInterval(kdim, curlvl + 1, min_datavec, max_datavec);
                else return this->left->findUsersetInterval(kdim, curlvl + 1, min_datavec, max_datavec);

                // Else: Interval extends over left and right.
            } else { // Split interval [min,max] into two: [min,...] [...,max]
                //printf("split\n");
                //printf("splitting at level %i for dimension %i\n", curlvl, index);
                std::list<VALUE>* ret = new std::list<VALUE>;

                // If there is data on the left side of the tree then collect.
                if (this->left != NULL) {
                    // take left half-interval.
                    float* left_min = new float[kdim];
                    float* left_max = new float[kdim];
                    memcpy(left_min, min_datavec, sizeof (float) * kdim);
                    memcpy(left_max, max_datavec, sizeof (float) * kdim);
                    left_max[index] = this->comparevalue - epsilon; // sure less than.
                    std::list<VALUE>* left_list = this->left->findUsersetInterval(kdim, curlvl + 1, left_min, left_max);
                    delete left_min;
                    delete left_max;
                    ret->merge(*left_list, no_specific_order<VALUE>());
                    delete left_list;
                }

                // If there is data on the right side of the tree then collect.
                if (this->right != NULL) {
                    // take right half-interval.
                    float* right_min = new float[kdim];
                    float* right_max = new float[kdim];
                    memcpy(right_min, min_datavec, sizeof (float) * kdim);
                    memcpy(right_max, max_datavec, sizeof (float) * kdim);
                    right_min[index] = this->comparevalue; // sure greater or equal than.
                    std::list<VALUE>* right_list = this->right->findUsersetInterval(kdim, curlvl + 1, right_min, right_max);
                    delete right_min;
                    delete right_max;
                    ret->merge(*right_list, no_specific_order<VALUE>());
                    delete right_list;
                }

                //printf("joining at level %i for dimension %i\n", curlvl, index);
                //printf("merging two lists: %i + %i = ", left->size(), right->size());
                //printf(" %i\n", ret->size());

                return ret;
            } // split interval.
        } // KDNode->kd_findUsersetInterval()

        /**
         * Finds a set of similar or equal vectors to the given data-vector.
         * If the returned set contains more than one vector then they are equal
         * as for the data but the pointers are different (Data-Vectors do
         * have data+identity). Don't delete returned sets!! Returned sets are
         * part of the kd-tree!
         */
        std::list<float*>* findDataset(unsigned int kdim, unsigned int curlvl, float* datavec) {

            if (this->left == NULL && this->right == NULL) return this->dataset;

            // Current dimension for clustering
            unsigned int index = curlvl % kdim;

            float value = datavec[index];
            if (value < this->comparevalue) {
                // If the data for the dimension is the same then only
                // either left or right will contain the data.
                if (this->left != NULL) return this->left->findDataset(kdim, curlvl + 1, datavec);
                else return this->right->findDataset(kdim, curlvl + 1, datavec);
            } else {
                // If the data for the dimension is the same then only
                // either left or right will contain the data.
                if (this->right != NULL) return this->right->findDataset(kdim, curlvl + 1, datavec);
                else return this->left->findDataset(kdim, curlvl + 1, datavec);
            }
        } // KDNode->findDataset()

        /**
         * Finds a set of similar or equal vectors to the given data-vector and
         * returns their associated userdata (objects related to those vectors).
         * If the returned set contains more than one vector then they are equal
         * as for the data but the pointers are different (Data-Vectors do
         * have data+identity). Don't delete returned sets!! Returned sets are
         * part of the kd-tree!
         */
        std::list<VALUE>* findUserset(unsigned int kdim, unsigned int curlvl, float* datavec) {

            if (this->left == NULL && this->right == NULL) return this->userset;

            // Current dimension for clustering
            unsigned int index = curlvl % kdim;

            float value = datavec[index];
            if (value < this->comparevalue) {
                // If the data for the dimension is the same then only
                // either left or right will contain the data.
                if (this->left != NULL) return this->left->findUserset(kdim, curlvl + 1, datavec);
                else return this->right->findUserset(kdim, curlvl + 1, datavec);
            } else {
                // If the data for the dimension is the same then only
                // either left or right will contain the data.
                if (this->right != NULL) return this->right->findUserset(kdim, curlvl + 1, datavec);
                else return this->left->findUserset(kdim, curlvl + 1, datavec);
            }
        } // KDNode->findUserset()

    }; // KDNode

}; // kdtree


#endif	/* _KDTREE_H */


