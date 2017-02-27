//
// Created by han on 16-11-15.
//
#include "NFindCorners.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#define DEBUG_CORNERS

namespace NFindCorners {
  struct CvCBCorner
  {
    CvPoint2D32f pt; // Coordinates of the corner
    int row;         // Board row index
    int count;       // Number of neighbor corners
    struct CvCBCorner* neighbors[4]; // Neighbor corners

    float meanDist(int *_n) const
    {
      float sum = 0;
      int n = 0;
      for( int i = 0; i < 4; i++ )
        {
          if( neighbors[i] )
            {
              float dx = neighbors[i]->pt.x - pt.x;
              float dy = neighbors[i]->pt.y - pt.y;
              sum += sqrt(dx*dx + dy*dy);
              n++;
            }
        }
      if(_n)
        *_n = n;
      return sum/MAX(n,1);
    }
  };
  /// Quadrangle contour info structure
  /** This structure stores information about the chessboard quadrange.*/
  struct CvCBQuad
  {
    int count;      // Number of quad neighbors
    int group_idx;  // quad group ID
    int row, col;   // row and column of this quad
    bool ordered;   // true if corners/neighbors are ordered counter-clockwise
    float edge_len; // quad edge len, in pix^2
    // neighbors and corners are synced, i.e., neighbor 0 shares corner 0
    CvCBCorner *corners[4]; // Coordinates of quad corners
    struct CvCBQuad *neighbors[4]; // Pointers of quad neighbors
  };

  struct CvContourEx
  {
    CV_CONTOUR_FIELDS()
    int counter;
  };

  static void SHOW_QUADS(const std::string & name, const Mat & img_, CvCBQuad * quads, int quads_count)
  {
    Mat img = img_.clone();
    if (img.channels() == 1)
      cvtColor(img, img, COLOR_GRAY2BGR);
    for (int i = 0; i < quads_count; ++i)
      {
        CvCBQuad & quad = quads[i];
        for (int j = 0; j < 4; ++j)
          {
            line(img, quad.corners[j]->pt, quad.corners[(j + 1) % 4]->pt, Scalar(0, 240, 0), 1, 16);
          }
      }
    imshow(name, img);
    while ((uchar)waitKey(0) != 'q') {}
  }


  int icvGetIntensityHistogram( const Mat & img, std::vector<int>& piHist )
  {
    // sum up all pixel in row direction and divide by number of columns
    for ( int j=0; j<img.rows; j++ )
      {
        const uchar * row = img.ptr(j);
        for ( int i=0; i<img.cols; i++ )
          {
            piHist[row[i]]++;
          }
      }
    return 0;
  }

  int icvSmoothHistogram( const std::vector<int>& piHist, std::vector<int>& piHistSmooth, int iWidth )
  {
    int iIdx;
    for ( int i=0; i<256; i++)
      {
        int iSmooth = 0;
        for ( int ii=-iWidth; ii<=iWidth; ii++)
          {
            iIdx = i+ii;
            if (iIdx > 0 && iIdx < 256)
              {
                iSmooth += piHist[iIdx];
              }
          }
        piHistSmooth[i] = iSmooth/(2*iWidth+1);
      }
    return 0;
  }

  //COMPUTE FAST HISTOGRAM GRADIENT
  int icvGradientOfHistogram( const std::vector<int>& piHist, std::vector<int>& piHistGrad )
  {
    piHistGrad[0] = 0;
    for ( int i=1; i<255; i++)
      {
        piHistGrad[i] = piHist[i-1] - piHist[i+1];
        if ( abs(piHistGrad[i]) < 100 )
          {
            if ( piHistGrad[i-1] == 0)
              piHistGrad[i] = -100;
            else
              piHistGrad[i] = piHistGrad[i-1];
          }
      }
    return 0;
  }

  //PERFORM SMART IMAGE THRESHOLDING BASED ON ANALYSIS OF INTENSTY HISTOGRAM
  bool icvBinarizationHistogramBased( Mat & img )
  {
    CV_Assert(img.channels() == 1 && img.depth() == CV_8U);
    int iCols = img.cols;
    int iRows = img.rows;
    int iMaxPix = iCols*iRows;
    int iMaxPix1 = iMaxPix/100;
    const int iNumBins = 256;
    std::vector<int> piHistIntensity(iNumBins, 0);
    std::vector<int> piHistSmooth(iNumBins, 0);
    std::vector<int> piHistGrad(iNumBins, 0);
    std::vector<int> piAccumSum(iNumBins, 0);
    std::vector<int> piMaxPos(20, 0);
    int iThresh = 0;
    int iIdx;
    int iWidth = 1;

    icvGetIntensityHistogram( img, piHistIntensity );

    // get accumulated sum starting from bright
    piAccumSum[iNumBins-1] = piHistIntensity[iNumBins-1];
    for ( int i=iNumBins-2; i>=0; i-- )
      {
        piAccumSum[i] = piHistIntensity[i] + piAccumSum[i+1];
      }

    // first smooth the distribution
    icvSmoothHistogram( piHistIntensity, piHistSmooth, iWidth );

    // compute gradient
    icvGradientOfHistogram( piHistSmooth, piHistGrad );

    // check for zeros
    int iCntMaxima = 0;
    for ( int i=iNumBins-2; (i>2) && (iCntMaxima<20); i--)
      {
        if ( (piHistGrad[i-1] < 0) && (piHistGrad[i] > 0) )
          {
            piMaxPos[iCntMaxima] = i;
            iCntMaxima++;
          }
      }

    iIdx = 0;
    int iSumAroundMax = 0;
    for ( int i=0; i<iCntMaxima; i++ )
      {
        iIdx = piMaxPos[i];
        iSumAroundMax = piHistSmooth[iIdx-1] + piHistSmooth[iIdx] + piHistSmooth[iIdx+1];
        if ( iSumAroundMax < iMaxPix1 && iIdx < 64 )
          {
            for ( int j=i; j<iCntMaxima-1; j++ )
              {
                piMaxPos[j] = piMaxPos[j+1];
              }
            iCntMaxima--;
            i--;
          }
      }
    if ( iCntMaxima == 1)
      {
        iThresh = piMaxPos[0]/2;
      }
    else if ( iCntMaxima == 2)
      {
        iThresh = (piMaxPos[0] + piMaxPos[1])/2;
      }
    else // iCntMaxima >= 3
      {
        // CHECKING THRESHOLD FOR WHITE
        int iIdxAccSum = 0, iAccum = 0;
        for (int i=iNumBins-1; i>0; i--)
          {
            iAccum += piHistIntensity[i];
            // iMaxPix/18 is about 5,5%, minimum required number of pixels required for white part of chessboard
            if ( iAccum > (iMaxPix/18) )
              {
                iIdxAccSum = i;
                break;
              }
          }

        int iIdxBGMax = 0;
        int iBrightMax = piMaxPos[0];
        // printf("iBrightMax = %d\n", iBrightMax);
        for ( int n=0; n<iCntMaxima-1; n++)
          {
            iIdxBGMax = n+1;
            if ( piMaxPos[n] < iIdxAccSum )
              {
                break;
              }
            iBrightMax = piMaxPos[n];
          }

        // CHECKING THRESHOLD FOR BLACK
        int iMaxVal = piHistIntensity[piMaxPos[iIdxBGMax]];

        //IF TOO CLOSE TO 255, jump to next maximum
        if ( piMaxPos[iIdxBGMax] >= 250 && iIdxBGMax < iCntMaxima )
          {
            iIdxBGMax++;
            iMaxVal = piHistIntensity[piMaxPos[iIdxBGMax]];
          }

        for ( int n=iIdxBGMax + 1; n<iCntMaxima; n++)
          {
            if ( piHistIntensity[piMaxPos[n]] >= iMaxVal )
              {
                iMaxVal = piHistIntensity[piMaxPos[n]];
                iIdxBGMax = n;
              }
          }

        //SETTING THRESHOLD FOR BINARIZATION
        int iDist2 = (iBrightMax - piMaxPos[iIdxBGMax])/2;
        iThresh = iBrightMax - iDist2;
        //PRINTF("THRESHOLD SELECTED = %d, BRIGHTMAX = %d, DARKMAX = %d\n", iThresh, iBrightMax, piMaxPos[iIdxBGMax]);
      }


    if ( iThresh > 0 )
      {
        for ( int jj=0; jj<iRows; jj++)
          {
            uchar * row = img.ptr(jj);
            for ( int ii=0; ii<iCols; ii++)
              {
                if ( row[ii] < iThresh )
                  row[ii] = 0;
                else
                  row[ii] = 255;
              }
          }
      }

    return true;
  }

#define MAX_CONTOUR_APPROX  7
  // returns corners in clockwise order
  // corners don't necessarily start at same position on quad (e.g.,
  //   top left corner)
  int icvGenerateQuads( CvCBQuad **out_quads, CvCBCorner **out_corners,
                        CvMemStorage *storage, const cv::Mat & image_, int *max_quad_buf_size,
                        double& rotAngle )
  {
    CvMat image_old(image_), *image = &image_old;
    int quad_count = 0;

    if( out_quads )
      *out_quads = 0;

    if( out_corners )
      *out_corners = 0;

    CvSeq *src_contour = 0;
    CvSeq *root;
    CvContourEx* board = 0;
    CvContourScanner scanner;
    int i, idx, min_size;

    CV_Assert( out_corners && out_quads );

    // empiric bound for minimal allowed perimeter for squares
    min_size = 600; //cvRound( image->cols * image->rows * .03 * 0.01 * 0.92 );

    // create temporary storage for contours and the sequence of pointers to found quadrangles
    cv::Ptr<CvMemStorage> temp_storage( cvCreateChildMemStorage( storage ) );
    root = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvSeq*), temp_storage );

    // initialize contour retrieving routine
    scanner = cvStartFindContours( image, temp_storage, sizeof(CvContourEx),
                                   CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

    // get all the contours one by one
    while( (src_contour = cvFindNextContour( scanner )) != 0 )
      {
        CvSeq *dst_contour = 0;
        CvRect rect = ((CvContour*)src_contour)->rect;

        // reject contours with too small perimeter
        if( CV_IS_SEQ_HOLE(src_contour) && rect.width*rect.height >= min_size )
          {
            const int min_approx_level = 1, max_approx_level = MAX_CONTOUR_APPROX;
            int approx_level;
            for( approx_level = min_approx_level; approx_level <= max_approx_level; approx_level++ )
              {
                dst_contour = cvApproxPoly( src_contour, sizeof(CvContour), temp_storage,
                                            CV_POLY_APPROX_DP, (float)approx_level );
                if( dst_contour->total == 4 )
                  break;

                // we call this again on its own output, because sometimes
                // cvApproxPoly() does not simplify as much as it should.
                dst_contour = cvApproxPoly( dst_contour, sizeof(CvContour), temp_storage,
                                            CV_POLY_APPROX_DP, (float)approx_level );

                if( dst_contour->total == 4 )
                  break;
              }

            // reject non-quadrangles
            if( dst_contour->total == 4 && cvCheckContourConvexity(dst_contour) )
              {
                CvPoint pt[4];
                double d1, d2, p = cvContourPerimeter(dst_contour);
                double area = fabs(cvContourArea(dst_contour, CV_WHOLE_SEQ));
                double dx, dy;

                for( i = 0; i < 4; i++ )
                  pt[i] = *(CvPoint*)cvGetSeqElem(dst_contour, i);

                dx = pt[0].x - pt[2].x;
                dy = pt[0].y - pt[2].y;
                d1 = sqrt(dx*dx + dy*dy);

                dx = pt[1].x - pt[3].x;
                dy = pt[1].y - pt[3].y;
                d2 = sqrt(dx*dx + dy*dy);

                // philipg.  Only accept those quadrangles which are more square
                // than rectangular and which are big enough
                double d3, d4;
                dx = pt[0].x - pt[1].x;
                dy = pt[0].y - pt[1].y;
                d3 = sqrt(dx*dx + dy*dy);
                dx = pt[1].x - pt[2].x;
                dy = pt[1].y - pt[2].y;
                d4 = sqrt(dx*dx + dy*dy);
                if( (d3*4 > d4 && d4*4 > d3 && d3*d4 < area*1.5 && area > min_size &&
                     d1 >= 0.15 * p && d2 >= 0.15 * p) )
                  {
                    CvContourEx* parent = (CvContourEx*)(src_contour->v_prev);
                    parent->counter++;
                    if( !board || board->counter < parent->counter )
                      board = parent;
                    dst_contour->v_prev = (CvSeq*)parent;
                    //for( i = 0; i < 4; i++ ) cvLine( debug_img, pt[i], pt[(i+1)&3], cvScalar(200,255,255), 1, CV_AA, 0 );
                    cvSeqPush( root, &dst_contour );
                  }
              }
          }
      }

    // finish contour retrieving
    cvEndFindContours( &scanner );

    // allocate quad & corner buffers
    *max_quad_buf_size = MAX(1, (root->total+root->total / 2)) * 2;
    *out_quads = (CvCBQuad*)cvAlloc(*max_quad_buf_size * sizeof((*out_quads)[0]));
    *out_corners = (CvCBCorner*)cvAlloc(*max_quad_buf_size * 4 * sizeof((*out_corners)[0]));


    // Add to find the max quad.
    /* For finding max length.
       float maxLen = 0;
       int maxQuadNum = 0;*/

    float minDist = FLT_MAX;
    int minQuadNum = 0;
    Point2f imageCenter(image_.cols/2.0, image_.rows/2.0);

    // Create array of quads structures
    for( idx = 0; idx < root->total; idx++ )
      {
        CvCBQuad* q = &(*out_quads)[quad_count];
        src_contour = *(CvSeq**)cvGetSeqElem( root, idx );
        //if( (flags & CV_CALIB_CB_FILTER_QUADS) && src_contour->v_prev != (CvSeq*)board )
        //    continue;

        // reset group ID
        memset( q, 0, sizeof(*q) );
        q->group_idx = -1;
        assert( src_contour->total == 4 );
        for( i = 0; i < 4; i++ )
          {
            CvPoint2D32f pt = cvPointTo32f(*(CvPoint*)cvGetSeqElem(src_contour, i));
            CvCBCorner* corner = &(*out_corners)[quad_count*4 + i];

            memset( corner, 0, sizeof(*corner) );
            corner->pt = pt;
            q->corners[i] = corner;
          }
        q->edge_len = FLT_MAX;
        for( i = 0; i < 4; i++ )
          {
            float dx = q->corners[i]->pt.x - q->corners[(i+1)&3]->pt.x;
            float dy = q->corners[i]->pt.y - q->corners[(i+1)&3]->pt.y;
            float d = dx*dx + dy*dy;
            if( q->edge_len > d )
              q->edge_len = d;
          }

        //--
        Point2f qCenter((q->corners[0]->pt.x+q->corners[2]->pt.x)/2.0,
                        (q->corners[0]->pt.y+q->corners[2]->pt.y)/2.0);
        float tempDist = fabs(imageCenter.x-qCenter.x) + fabs(imageCenter.y-qCenter.y);
        if(tempDist < minDist){
          minDist = tempDist;
          minQuadNum = quad_count;
        }

        quad_count++;
      }

    CvCBQuad* maxQuad = &(*out_quads)[minQuadNum];
    if(minDist<FLT_MAX) {
      double angle1 = atan2(maxQuad->corners[1]->pt.y-maxQuad->corners[0]->pt.y ,
                            maxQuad->corners[1]->pt.x-maxQuad->corners[0]->pt.x);

      double angle2 = atan2(maxQuad->corners[2]->pt.y-maxQuad->corners[1]->pt.y ,
                            maxQuad->corners[2]->pt.x-maxQuad->corners[1]->pt.x);

      if(fabs(angle1) < fabs(angle2)) {
        rotAngle = angle1;
      } else {
        rotAngle = angle2;
      }
    }

    return quad_count;
  }

  void icvFindQuadNeighbors( CvCBQuad *quads, int quad_count )
  {
    const float thresh_scale = 1.f;
    int idx, i, k, j;
    float dx, dy, dist;

    // find quad neighbors
    for( idx = 0; idx < quad_count; idx++ )
      {
        CvCBQuad* cur_quad = &quads[idx];

        // choose the points of the current quadrangle that are close to
        // some points of the other quadrangles
        // (it can happen for split corners (due to dilation) of the
        // checker board). Search only in other quadrangles!

        // for each corner of this quadrangle
        for( i = 0; i < 4; i++ )
          {
            CvPoint2D32f pt;
            float min_dist = FLT_MAX;
            int closest_corner_idx = -1;
            CvCBQuad *closest_quad = 0;
            CvCBCorner *closest_corner = 0;

            if( cur_quad->neighbors[i] )
              continue;

            pt = cur_quad->corners[i]->pt;

            // find the closest corner in all other quadrangles
            for( k = 0; k < quad_count; k++ )
              {
                if( k == idx )
                  continue;

                for( j = 0; j < 4; j++ )
                  {
                    if( quads[k].neighbors[j] )
                      continue;

                    dx = pt.x - quads[k].corners[j]->pt.x;
                    dy = pt.y - quads[k].corners[j]->pt.y;
                    dist = dx * dx + dy * dy;

                    if( dist < min_dist &&
                        dist <= cur_quad->edge_len*thresh_scale &&
                        dist <= quads[k].edge_len*thresh_scale )
                      {
                        // check edge lengths, make sure they're compatible
                        // edges that are different by more than 1:4 are rejected
                        float ediff = cur_quad->edge_len - quads[k].edge_len;
                        if (ediff > 32*cur_quad->edge_len ||
                            ediff > 32*quads[k].edge_len)
                          {
                            //PRINTF("Incompatible edge lengths\n");
                            continue;
                          }
                        closest_corner_idx = j;
                        closest_quad = &quads[k];
                        min_dist = dist;
                      }
                  }
              }

            // we found a matching corner point?
            if( closest_corner_idx >= 0 && min_dist < FLT_MAX )
              {
                // If another point from our current quad is closer to the found corner
                // than the current one, then we don't count this one after all.
                // This is necessary to support small squares where otherwise the wrong
                // corner will get matched to closest_quad;
                closest_corner = closest_quad->corners[closest_corner_idx];

                for( j = 0; j < 4; j++ )
                  {
                    if( cur_quad->neighbors[j] == closest_quad )
                      break;

                    dx = closest_corner->pt.x - cur_quad->corners[j]->pt.x;
                    dy = closest_corner->pt.y - cur_quad->corners[j]->pt.y;

                    if( dx * dx + dy * dy < min_dist )
                      break;
                  }

                if( j < 4 || cur_quad->count >= 4 || closest_quad->count >= 4 )
                  continue;

                // Check that each corner is a neighbor of different quads
                for( j = 0; j < closest_quad->count; j++ )
                  {
                    if( closest_quad->neighbors[j] == cur_quad )
                      break;
                  }
                if( j < closest_quad->count )
                  continue;

                // check whether the closest corner to closest_corner
                // is different from cur_quad->corners[i]->pt
                for( k = 0; k < quad_count; k++ )
                  {
                    CvCBQuad* q = &quads[k];
                    if( k == idx || q == closest_quad )
                      continue;

                    for( j = 0; j < 4; j++ )
                      if( !q->neighbors[j] )
                        {
                          dx = closest_corner->pt.x - q->corners[j]->pt.x;
                          dy = closest_corner->pt.y - q->corners[j]->pt.y;
                          dist = dx*dx + dy*dy;
                          if( dist < min_dist )
                            break;
                        }
                    if( j < 4 )
                      break;
                  }

                if( k < quad_count )
                  continue;

                closest_corner->pt.x = (pt.x + closest_corner->pt.x) * 0.5f;
                closest_corner->pt.y = (pt.y + closest_corner->pt.y) * 0.5f;

                // We've found one more corner - remember it
                cur_quad->count++;
                cur_quad->neighbors[i] = closest_quad;
                cur_quad->corners[i] = closest_corner;

                closest_quad->count++;
                closest_quad->neighbors[closest_corner_idx] = cur_quad;
              }
          }
      }
  }

  int icvFindConnectedQuads( CvCBQuad *quad, int quad_count, CvCBQuad **out_group,
                             int group_idx, CvMemStorage* storage )
  {
    cv::Ptr<CvMemStorage> temp_storage(cvCreateChildMemStorage( storage ));
    CvSeq* stack = cvCreateSeq( 0, sizeof(*stack), sizeof(void*), temp_storage );
    int i, count = 0;

    // Scan the array for a first unlabeled quad
    for( i = 0; i < quad_count; i++ )
      {
        if( quad[i].count > 0 && quad[i].group_idx < 0)
          break;
      }

    // Recursively find a group of connected quads starting from the seed quad[i]
    if( i < quad_count )
      {
        CvCBQuad* q = &quad[i];
        cvSeqPush( stack, &q );
        out_group[count++] = q;
        q->group_idx = group_idx;
        q->ordered = false;

        while( stack->total )
          {
            cvSeqPop( stack, &q );
            for( i = 0; i < 4; i++ )
              {
                CvCBQuad *neighbor = q->neighbors[i];
                if( neighbor && neighbor->count > 0 && neighbor->group_idx < 0 )
                  {
                    cvSeqPush( stack, &neighbor );
                    out_group[count++] = neighbor;
                    neighbor->group_idx = group_idx;
                    neighbor->ordered = false;
                  }
              }
          }
      }

    return count;
  }

  void icvOrderQuad(CvCBQuad *quad, CvCBCorner *corner, int common)
  {
    // find the corner
    int tc;
    for (tc=0; tc<4; tc++)
      if (quad->corners[tc]->pt.x == corner->pt.x &&
          quad->corners[tc]->pt.y == corner->pt.y)
        break;

    // set corner order
    // shift
    while (tc != common)
      {
        // shift by one
        CvCBCorner *tempc;
        CvCBQuad *tempq;
        tempc = quad->corners[3];
        tempq = quad->neighbors[3];
        for (int i=3; i>0; i--)
          {
            quad->corners[i] = quad->corners[i-1];
            quad->neighbors[i] = quad->neighbors[i-1];
          }
        quad->corners[0] = tempc;
        quad->neighbors[0] = tempq;
        tc++;
        tc = tc%4;
      }
  }


  // add an outer quad
  // looks for the neighbor of <quad> that isn't present,
  //   tries to add it in.
  // <quad> is ordered
  int icvAddOuterQuad( CvCBQuad *quad, CvCBQuad **quads, int quad_count,
                       CvCBQuad **all_quads, int all_count, CvCBCorner **corners, int max_quad_buf_size )

  {
    int added = 0;
    for (int i=0; i<4 && all_count < max_quad_buf_size; i++) // find no-neighbor corners
      {
        if (!quad->neighbors[i])    // ok, create and add neighbor
          {
            int j = (i+2)%4;
            //PRINTF("Adding quad as neighbor 2\n");
            CvCBQuad *q = &(*all_quads)[all_count];
            memset( q, 0, sizeof(*q) );
            added++;
            quads[quad_count] = q;
            quad_count++;

            // set neighbor and group id
            quad->neighbors[i] = q;
            quad->count += 1;
            q->neighbors[j] = quad;
            q->group_idx = quad->group_idx;
            q->count = 1;   // number of neighbors
            q->ordered = false;
            q->edge_len = quad->edge_len;

            // make corners of new quad
            // same as neighbor quad, but offset
            CvPoint2D32f pt = quad->corners[i]->pt;
            CvCBCorner* corner;
            float dx = pt.x - quad->corners[j]->pt.x;
            float dy = pt.y - quad->corners[j]->pt.y;
            for (int k=0; k<4; k++)
              {
                corner = &(*corners)[all_count*4+k];
                pt = quad->corners[k]->pt;
                memset( corner, 0, sizeof(*corner) );
                corner->pt = pt;
                q->corners[k] = corner;
                corner->pt.x += dx;
                corner->pt.y += dy;
              }
            // have to set exact corner
            q->corners[j] = quad->corners[i];

            // now find other neighbor and add it, if possible
            if (quad->neighbors[(i+3)%4] &&
                quad->neighbors[(i+3)%4]->ordered &&
                quad->neighbors[(i+3)%4]->neighbors[i] &&
                quad->neighbors[(i+3)%4]->neighbors[i]->ordered )
              {
                CvCBQuad *qn = quad->neighbors[(i+3)%4]->neighbors[i];
                q->count = 2;
                q->neighbors[(j+1)%4] = qn;
                qn->neighbors[(i+1)%4] = q;
                qn->count += 1;
                // have to set exact corner
                q->corners[(j+1)%4] = qn->corners[(i+1)%4];
              }

            all_count++;
          }
      }
    return added;
  }

  void icvRemoveQuadFromGroup(CvCBQuad **quads, int count, CvCBQuad *q0)
  {
    int i, j;
    // remove any references to this quad as a neighbor
    for(i = 0; i < count; i++ )
      {
        CvCBQuad *q = quads[i];
        for(j = 0; j < 4; j++ )
          {
            if( q->neighbors[j] == q0 )
              {
                q->neighbors[j] = 0;
                q->count--;
                for(int k = 0; k < 4; k++ )
                  if( q0->neighbors[k] == q )
                    {
                      q0->neighbors[k] = 0;
                      q0->count--;
                      break;
                    }
                break;
              }
          }
      }

    // remove the quad
    for(i = 0; i < count; i++ )
      {
        CvCBQuad *q = quads[i];
        if (q == q0)
          {
            quads[i] = quads[count-1];
            break;
          }
      }
  }


  //
  // order a group of connected quads
  // order of corners:
  //   0 is top left
  //   clockwise from there
  // note: "top left" is nominal, depends on initial ordering of starting quad
  //   but all other quads are ordered consistently
  //
  // can change the number of quads in the group
  // can add quads, so we need to have quad/corner arrays passed in
  //
  int icvOrderFoundConnectedQuads( int quad_count, CvCBQuad **quads,
                                   int *all_count, CvCBQuad **all_quads, CvCBCorner **corners,
                                   CvSize pattern_size, int max_quad_buf_size, CvMemStorage* storage )
  {
    cv::Ptr<CvMemStorage> temp_storage(cvCreateChildMemStorage( storage ));
    CvSeq* stack = cvCreateSeq( 0, sizeof(*stack), sizeof(void*), temp_storage );

    // first find an interior quad
    CvCBQuad *start = NULL;
    for (int i=0; i<quad_count; i++)
      {
        if (quads[i]->count == 4)
          {
            start = quads[i];
            break;
          }
      }

    if (start == NULL)
      return 0;   // no 4-connected quad

    // start with first one, assign rows/cols
    int row_min = 0, col_min = 0, row_max=0, col_max = 0;

    std::map<int, int> col_hist;
    std::map<int, int> row_hist;

    cvSeqPush(stack, &start);
    start->row = 0;
    start->col = 0;
    start->ordered = true;

    // Recursively order the quads so that all position numbers (e.g.,
    // 0,1,2,3) are in the at the same relative corner (e.g., lower right).
    while( stack->total )
      {
        CvCBQuad* q;
        cvSeqPop( stack, &q );
        int col = q->col;
        int row = q->row;
        col_hist[col]++;
        row_hist[row]++;

        // check min/max
        if (row > row_max) row_max = row;
        if (row < row_min) row_min = row;
        if (col > col_max) col_max = col;
        if (col < col_min) col_min = col;

        for(int i = 0; i < 4; i++ )
          {
            CvCBQuad *neighbor = q->neighbors[i];
            switch(i)   // adjust col, row for this quad
              {           // start at top left, go clockwise
              case 0:
                row--; col--; break;
              case 1:
                col += 2; break;
              case 2:
                row += 2;   break;
              case 3:
                col -= 2; break;
              }

            // just do inside quads
            if (neighbor && neighbor->ordered == false && neighbor->count == 4)
              {
                //PRINTF("col: %d  row: %d\n", col, row);
                icvOrderQuad(neighbor, q->corners[i], (i+2)%4); // set in order
                neighbor->ordered = true;
                neighbor->row = row;
                neighbor->col = col;
                cvSeqPush( stack, &neighbor );
              }
          }
      }

    // analyze inner quad structure
    int w = pattern_size.width - 1;
    int h = pattern_size.height - 1;
    int drow = row_max - row_min + 1;
    int dcol = col_max - col_min + 1;
    //cout << "drow: " <<drow <<endl;
    //cout << "dcol: " <<dcol <<endl;

    // normalize pattern and found quad indices
    if ((w > h && dcol < drow) ||
        (w < h && drow < dcol))
      {
        h = pattern_size.width - 1;
        w = pattern_size.height - 1;
      }

    //printf("Size: %dx%d  Pattern: %dx%d\n", dcol, drow, w, h);

    // check if there are enough inner quads
    if (dcol < w || drow < h)   // found enough inner quads?
      {
        //PRINTF("Too few inner quad rows/cols\n");
        //return 0;   // no, return
      }
#ifdef ENABLE_TRIM_COL_ROW
    // too many columns, not very common
    if (dcol == w+1)    // too many, trim
      {
        PRINTF("Trimming cols\n");
        if (col_hist[col_max] > col_hist[col_min])
          {
            PRINTF("Trimming left col\n");
            quad_count = icvTrimCol(quads,quad_count,col_min,-1);
          }
        else
          {
            PRINTF("Trimming right col\n");
            quad_count = icvTrimCol(quads,quad_count,col_max,+1);
          }
      }

    // too many rows, not very common
    if (drow == h+1)    // too many, trim
      {
        PRINTF("Trimming rows\n");
        if (row_hist[row_max] > row_hist[row_min])
          {
            PRINTF("Trimming top row\n");
            quad_count = icvTrimRow(quads,quad_count,row_min,-1);
          }
        else
          {
            PRINTF("Trimming bottom row\n");
            quad_count = icvTrimRow(quads,quad_count,row_max,+1);
          }
      }
#endif

    // check edges of inner quads
    // if there is an outer quad missing, fill it in
    // first order all inner quads
    int found = 0;
    for (int i=0; i<quad_count; i++)
      {
        if (quads[i]->count == 4)
          {   // ok, look at neighbors
            int col = quads[i]->col;
            int row = quads[i]->row;
            for (int j=0; j<4; j++)
              {
                switch(j)   // adjust col, row for this quad
                  {       // start at top left, go clockwise
                  case 0:
                    row--; col--; break;
                  case 1:
                    col += 2; break;
                  case 2:
                    row += 2;   break;
                  case 3:
                    col -= 2; break;
                  }
                CvCBQuad *neighbor = quads[i]->neighbors[j];
                if (neighbor && !neighbor->ordered && // is it an inner quad?
                    col <= col_max && col >= col_min &&
                    row <= row_max && row >= row_min)
                  {
                    // if so, set in order
                    //PRINTF("Adding inner: col: %d  row: %d\n", col, row);
                    found++;
                    icvOrderQuad(neighbor, quads[i]->corners[j], (j+2)%4);
                    neighbor->ordered = true;
                    neighbor->row = row;
                    neighbor->col = col;
                  }
              }
          }
      }

    // if we have found inner quads, add corresponding outer quads,
    //   which are missing
    if (found > 0)
      {
        //PRINTF("Found %d inner quads not connected to outer quads, repairing\n", found);

        for (int i=0; i<quad_count && *all_count < max_quad_buf_size; i++)
          {
            if (quads[i]->count < 4 && quads[i]->ordered)
              {
                int added = icvAddOuterQuad(quads[i],quads,quad_count,all_quads,*all_count,corners, max_quad_buf_size);
                *all_count += added;
                quad_count += added;
              }
          }

        if (*all_count >= max_quad_buf_size)
          return 0;
      }


    // final trimming of outer quads
    //if (dcol == w && drow == h) // found correct inner quads
    {
      //PRINTF("Inner bounds ok, check outer quads\n");
      int rcount = quad_count;
      for (int i=quad_count-1; i>=0; i--) // eliminate any quad not connected to
        // an ordered quad
        {
          if (quads[i]->ordered == false)
            {
              bool outer = false;
              for (int j=0; j<4; j++) // any neighbors that are ordered?
                {
                  if (quads[i]->neighbors[j] && quads[i]->neighbors[j]->ordered)
                    outer = true;
                }
              if (!outer) // not an outer quad, eliminate
                {
                  //PRINTF("Removing quad %d\n", i);
                  icvRemoveQuadFromGroup(quads,rcount,quads[i]);
                  rcount--;
                }
            }

        }
      return rcount;
    }

    return 0;
  }


  int icvCheckQuadGroup( CvCBQuad **quad_group, int quad_count,
                         CvCBCorner **out_corners, CvSize pattern_size )
  {
    const int ROW1 = 1000000;
    const int ROW2 = 2000000;
    const int ROW_ = 3000000;
    int result = 0;
    int i, out_corner_count = 0, corner_count = 0;
    std::vector<CvCBCorner*> corners(quad_count*4);

    int j, k, kk;
    int width = 0, height = 0;
    int hist[5] = {0,0,0,0,0};
    CvCBCorner* first = 0, *first2 = 0, *right, *cur, *below, *c;

    // build dual graph, which vertices are internal quad corners
    // and two vertices are connected iff they lie on the same quad edge
    for( i = 0; i < quad_count; i++ )
      {
        CvCBQuad* q = quad_group[i];
        /*CvScalar color = q->count == 0 ? cvScalar(0,255,255) :
          q->count == 1 ? cvScalar(0,0,255) :
          q->count == 2 ? cvScalar(0,255,0) :
          q->count == 3 ? cvScalar(255,255,0) :
          cvScalar(255,0,0);*/

        for( j = 0; j < 4; j++ )
          {
            //cvLine( debug_img, cvPointFrom32f(q->corners[j]->pt), cvPointFrom32f(q->corners[(j+1)&3]->pt), color, 1, CV_AA, 0 );
            if( q->neighbors[j] )
              {
                CvCBCorner *a = q->corners[j], *b = q->corners[(j+1)&3];
                // mark internal corners that belong to:
                //   - a quad with a single neighbor - with ROW1,
                //   - a quad with two neighbors     - with ROW2
                // make the rest of internal corners with ROW_
                int row_flag = q->count == 1 ? ROW1 : q->count == 2 ? ROW2 : ROW_;

                if( a->row == 0 )
                  {
                    corners[corner_count++] = a;
                    a->row = row_flag;
                  }
                else if( a->row > row_flag )
                  a->row = row_flag;

                if( q->neighbors[(j+1)&3] )
                  {
                    if( a->count >= 4 || b->count >= 4 )
                      goto finalize;
                    for( k = 0; k < 4; k++ )
                      {
                        if( a->neighbors[k] == b )
                          goto finalize;
                        if( b->neighbors[k] == a )
                          goto finalize;
                      }
                    a->neighbors[a->count++] = b;
                    b->neighbors[b->count++] = a;
                  }
              }
          }
      }

    //if( corner_count != pattern_size.width*pattern_size.height )
    //   goto finalize;

    for( i = 0; i < corner_count; i++ )
      {
        int n = corners[i]->count;
        assert( 0 <= n && n <= 4 );
        hist[n]++;
        if( !first && n == 2 )
          {
            if( corners[i]->row == ROW1 )
              first = corners[i];
            else if( !first2 && corners[i]->row == ROW2 )
              first2 = corners[i];
          }
      }

    // start with a corner that belongs to a quad with a signle neighbor.
    // if we do not have such, start with a corner of a quad with two neighbors.
    if( !first )
      first = first2;

    if( !first || hist[0] != 0 || hist[1] != 0 || hist[2] != 4 )
      //|| hist[3] != (pattern_size.width + pattern_size.height)*2 - 8 )
      goto finalize;

    cur = first;
    right = below = 0;
    out_corners[out_corner_count++] = cur;

    for( k = 0; k < 4; k++ )
      {
        c = cur->neighbors[k];
        if( c )
          {
            if( !right )
              right = c;
            else if( !below )
              below = c;
          }
      }

    if( !right || (right->count != 2 && right->count != 3) ||
        !below || (below->count != 2 && below->count != 3) )
      goto finalize;

    cur->row = 0;
    //cvCircle( debug_img, cvPointFrom32f(cur->pt), 3, cvScalar(0,255,0), -1, 8, 0 );

    first = below; // remember the first corner in the next row
    // find and store the first row (or column)
    for(j=1;;j++)
      {
        right->row = 0;
        out_corners[out_corner_count++] = right;
        //cvCircle( debug_img, cvPointFrom32f(right->pt), 3, cvScalar(0,255-j*10,0), -1, 8, 0 );
        if( right->count == 2 )
          break;
        if( right->count != 3 )//|| out_corner_count >= MAX(pattern_size.width,pattern_size.height) )
          goto finalize;
        cur = right;
        for( k = 0; k < 4; k++ )
          {
            c = cur->neighbors[k];
            if( c && c->row > 0 )
              {
                for( kk = 0; kk < 4; kk++ )
                  {
                    if( c->neighbors[kk] == below )
                      break;
                  }
                if( kk < 4 )
                  below = c;
                else
                  right = c;
              }
          }
      }

    width = out_corner_count;
    if( width == pattern_size.width )
      height = pattern_size.height;
    else if( width == pattern_size.height )
      height = pattern_size.width;
    else
      goto finalize;

    // find and store all the other rows
    for( i = 1; ; i++ )
      {
        if( !first )
          break;
        cur = first;
        first = 0;
        for( j = 0;; j++ )
          {
            cur->row = i;
            out_corners[out_corner_count++] = cur;
            //cvCircle( debug_img, cvPointFrom32f(cur->pt), 3, cvScalar(0,0,255-j*10), -1, 8, 0 );
            //if( cur->count == 2 + (i < height-1) && j > 0 )
            //    break;

            right = 0;

            // find a neighbor that has not been processed yet
            // and that has a neighbor from the previous row
            for( k = 0; k < 4; k++ )
              {
                c = cur->neighbors[k];
                if( c && c->row > i )
                  {
                    for( kk = 0; kk < 4; kk++ )
                      {
                        if( c->neighbors[kk] && c->neighbors[kk]->row == i-1 )
                          break;
                      }
                    if( kk < 4 )
                      {
                        right = c;
                        if( j > 0 )
                          break;
                      }
                    else if( j == 0 )
                      first = c;
                  }
              }
            if( !right )
              goto finalize;
            cur = right;
          }

        //if( j != width - 1 )
        //    goto finalize;
      }

    if( out_corner_count != corner_count )
      goto finalize;

    /*
    // check if we need to transpose the board
    if( width != pattern_size.width )
    {
    CV_SWAP( width, height, k );

    memcpy( &corners[0], out_corners, corner_count*sizeof(corners[0]) );
    for( i = 0; i < height; i++ )
    for( j = 0; j < width; j++ )
    out_corners[i*width + j] = corners[j*height + i];
    }

    // check if we need to revert the order in each row
    {
    CvPoint2D32f p0 = out_corners[0]->pt, p1 = out_corners[pattern_size.width-1]->pt,
    p2 = out_corners[pattern_size.width]->pt;
    if( (p1.x - p0.x)*(p2.y - p1.y) - (p1.y - p0.y)*(p2.x - p1.x) < 0 )
    {
    if( width % 2 == 0 )
    {
    for( i = 0; i < height; i++ )
    for( j = 0; j < width/2; j++ )
    CV_SWAP( out_corners[i*width+j], out_corners[i*width+width-j-1], c );
    }
    else
    {
    for( j = 0; j < width; j++ )
    for( i = 0; i < height/2; i++ )
    CV_SWAP( out_corners[i*width+j], out_corners[(height - i - 1)*width+j], c );
    }
    }
    }
    */
    result = corner_count;

  finalize:

    if( result <= 0 )
      {
        //corner_count = MIN( corner_count, pattern_size.width*pattern_size.height );
        for( i = 0; i < corner_count; i++ )
          out_corners[i] = corners[i];
        result = -corner_count;

        //if (result == -pattern_size.width*pattern_size.height)
        //    result = -result;
      }

    return result;
  }


  bool processQuads(CvCBQuad *quads, int quad_count, CvSize pattern_size, int max_quad_buf_size,
                    CvMemStorage * storage, CvCBCorner *corners, CvPoint2D32f *out_corners, int *out_corner_count, int & prev_sqr_size)
  {

    if( quad_count <= 0 )
      return false;

    bool found = false;

    // Find quad's neighbors
    icvFindQuadNeighbors( quads, quad_count );

    // allocate extra for adding in icvOrderFoundQuads
    CvCBQuad **quad_group = 0;
    CvCBCorner **corner_group = 0;

    quad_group = (CvCBQuad**)cvAlloc( sizeof(quad_group[0]) * max_quad_buf_size);
    corner_group = (CvCBCorner**)cvAlloc( sizeof(corner_group[0]) * max_quad_buf_size * 4 );

    for( int group_idx = 0; ; group_idx++ )
      {
        int count = icvFindConnectedQuads( quads, quad_count, quad_group, group_idx, storage );

        if( count == 0 )
          break;

        // order the quad corners globally
        // maybe delete or add some
        //Starting ordering of inner quads
        count = icvOrderFoundConnectedQuads(count, quad_group, &quad_count, &quads, &corners,
                                            pattern_size, max_quad_buf_size, storage );
        //cout << "test: " <<group_idx << " " <<count <<endl;
        if (count == 0)
          continue;       // haven't found inner quads


        count = icvCheckQuadGroup( quad_group, count, corner_group, pattern_size );


        int n = count > 0 ? count: -count;//pattern_size.width * pattern_size.height : -count;
        //n = MIN( n, pattern_size.width * pattern_size.height );

        float sum_dist = 0;
        int total = 0;

        for(int i = 0; i < n; i++ )
          {
            int ni = 0;
            float avgi = corner_group[i]->meanDist(&ni);
            sum_dist += avgi*ni;
            total += ni;
          }
        prev_sqr_size = cvRound(sum_dist/MAX(total, 1));

        if( count > 0 || (out_corner_count && -count > *out_corner_count) )
          {
            // copy corners to output array
            for(int i = 0; i < n; i++ )
              out_corners[i] = corner_group[i]->pt;

            if( out_corner_count )
              *out_corner_count = n;

            if( *out_corner_count >= pattern_size.width*pattern_size.height &&
                *out_corner_count <= MAX_PATTERN.width*MAX_PATTERN.height)
              {
                found = true;
                break;
              }
          }
      }

    cvFree(&quad_group);
    cvFree(&corner_group);

    return found;
  }


  void plotHist(const Mat& mat, const char* str) {
    Mat hist;
    float range[] = {0,255};
    const float* histRange = {range};
    int histSize = 255;
    calcHist(&mat, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);

    int hist_h = 400;
    int hist_w = 400;
    int bin_w = cvRound(400.0/histSize);
    Mat histImage(400, 400, CV_8UC3, Scalar(0,0,0));
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

    for(int i=1; i<histSize; i++) {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
            Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
            Scalar( 0, 0, 255), 2, 8, 0  );
    }


    imshow(str, histImage);
    cout << "Showing Hist" <<endl;
    waitKey(0);

  }

  bool findCorners(Mat src, Size pattern_size, vector<Point2f>& out_corners, int& out_corner_count, int& prev_sqr_size, double& rotAngle) {

    //#define DEBUG_CORNERS
    Mat src_gray;
    cvtColor(src, src_gray, CV_BGR2GRAY);

    int minDilation=1;
    int maxDilation=7;

    bool found = false;
    CvCBQuad *quads = 0;
    CvCBCorner *corners = 0;
    cv::Ptr<CvMemStorage> storage(cvCreateMemStorage(0));
    //int prev_sqr_size = 0;
    int lastQuadsNum = 0;
    if (!found)
      {

        equalizeHist( src_gray, src_gray );

        Mat thresh_img;
        prev_sqr_size = 0;

        const bool useAdaptive = true;
        if (!useAdaptive)
          {
            // empiric threshold level
            // thresholding performed here and not inside the cycle to save processing time
            double mean = cv::mean(src_gray).val[0];
            int thresh_level = MAX(cvRound( mean - 10 ), 10);
            threshold( src_gray, thresh_img, thresh_level, 255, THRESH_BINARY );
          }

        //if flag CV_CALIB_CB_ADAPTIVE_THRESH is not set it doesn't make sense to iterate over k
        int max_k = useAdaptive ? 6 : 1;
        for(int k = 0; k < max_k; k++ )
          {
            if(found)
              break;
            for( int dilations = minDilation; dilations <= maxDilation; dilations++ )
              {
                // cout<<"dilations = "<<dilations<<endl;
                // convert the input grayscale image to binary (black-n-white)
                if (useAdaptive)
                  {
                    // cout<<"useAdaptive\n";
                    int block_size = cvRound(prev_sqr_size == 0
                                             ? MIN(src_gray.cols, src_gray.rows) * (k % 2 == 0 ? 0.2 : 0.1)
                                             : prev_sqr_size * 2);
                    block_size = block_size | 1;
                    // cout<<"block_size = "<<block_size<<endl;
                    // convert to binary
                    adaptiveThreshold( src_gray, thresh_img, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, block_size, (k/2)*5 );


                    if (dilations > 0) {
                      dilate( thresh_img, thresh_img, Mat(), Point(-1, -1), dilations-1 );
#ifdef DEBUG_CORNERS
                      imshow("binary", thresh_img);
                      waitKey(0);
#endif
                    }
                  }
                else
                  {
                    dilate( thresh_img, thresh_img, Mat(), Point(-1, -1), 1 );
                  }

                // So we can find rectangles that go to the edge, we draw a white line around the image edge.
                // Otherwise FindContours will miss those clipped rectangle contours.
                // The border color will be the image mean, because otherwise we risk screwing up filters like cvSmooth()...
                int max_quad_buf_size = 0;
                cvFree(&quads);
                cvFree(&corners);

                int quad_count = icvGenerateQuads( &quads, &corners, storage, thresh_img, &max_quad_buf_size, rotAngle);

#ifdef DEBUG_CORNERS
                cout << "fonud: " << found <<endl;
                cout << "quads_num: "<<quad_count <<endl;
                SHOW_QUADS("New quads", thresh_img, quads, quad_count);
#endif
                // Quads may be not under the same situation when the board is larger.
                if (found && quad_count<=lastQuadsNum) {
                  cout<<"strange found"<<endl;
                  break;      // already found it
                }
                lastQuadsNum = quad_count;

                if (processQuads(quads, quad_count, pattern_size, max_quad_buf_size, storage, corners, (CvPoint2D32f*)&out_corners[0], &out_corner_count, prev_sqr_size)) {
                  found = 1;
                }

              }
          }
      }

    return found;
  }
}
