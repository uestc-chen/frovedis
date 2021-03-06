package com.nec.frovedis.mllib.tree;

import com.nec.frovedis.Jexrpc.{FrovedisServer,JNISupport}
import com.nec.frovedis.mllib.{M_KIND,ModelID,GenericModelWithPredict}
import org.apache.spark.SparkContext
import com.nec.frovedis.matrix.ENUM
import org.apache.spark.rdd.RDD
import org.apache.spark.mllib.linalg.Vector

class DecisionTreeModel(val model_Id: Int,
                        val logic: Map[Double, Double]) 
  extends GenericModelWithPredict(model_Id, M_KIND.DTM){
  protected val enc_logic: Map[Double,Double] = logic

  override def predict(data: Vector) : Double = {
     val ret = super.predict(data)
     return if (enc_logic != null) enc_logic(ret) else ret
  }
  override def predict(data: RDD[Vector]) : RDD[Double] = {
    val ret = super.predict(data)
    return if (enc_logic != null) ret.map(x => enc_logic(x)) else ret
  }
  
  // to be implemented...
  /*
  def to_spark_model(sc: SparkContext): org.apache.spark.mllib.tree.model.DecisionTreeModel = {

  }
  */
}
 
object DecisionTreeModel {
  def load(sc: SparkContext, path: String): DecisionTreeModel = load(path)
  def load(path: String): DecisionTreeModel = {
    val modelId = ModelID.get()
    val fs = FrovedisServer.getServerInstance()
    JNISupport.loadFrovedisModel(fs.master_node,modelId,M_KIND.DTM,path)
    val info = JNISupport.checkServerException();
    if (info != "") throw new java.rmi.ServerException(info);
    return new DecisionTreeModel(modelId, null)
  }
}

