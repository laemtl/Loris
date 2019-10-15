<?php declare(strict_types=1);
/**
 * Pool Data Access Object (DAO) Class
 * This class contains all database handling that is needed to
 * permanently store and retrieve Pool Object instances
 *
 * PHP Version 7.2
 *
 * @category   DAO
 * @package    Loris
 * @subpackage Biobank
 * @author     Henri Rabalais <henri.rabalais@mcin.ca>
 * @license    http://www.gnu.org/licenses/gpl-3.0.txt GPLv3
 * @link       https://www.github.com/aces/Loris/
 */

namespace LORIS\biobank;

/**
 * Pool Data Access Object (DAO) Class
 * This class contains all database handling that is needed to
 * permanently store and retrieve Pool Object instances
 *
 * PHP Version 7.2
 *
 * @category   DAO
 * @package    Loris
 * @subpackage Biobank
 * @author     Henri Rabalais <henri.rabalais@mcin.ca>
 * @license    http://www.gnu.org/licenses/gpl-3.0.txt GPLv3
 * @link       https://www.github.com/aces/Loris/
 */

class PoolDAO extends \LORIS\Data\ProvisionerInstance
{
    /**
     * Private instance of the Database class.
     *
     * @var \Dabatase $dao
     */
    private $db;

    /**
     * Initializes a new instance of the PoolDAO Class
     *
     * @param Database $db Instance of the LORIS Database class
     */
    function __construct(\Database $db)
    {
        $this->db = $db;
    }

    // XXX: I'm not sure if this function is really necessary since it is only
    // being used once, and should likely not be accessed outside this class.
    /**
     * This method is used when the DAO class needs to create new Pool
     * instance.
     *
     * @return Pool
     */
    private function getInstance() : Pool
    {
        return new Pool();
    }

    /**
     * This will load all pool persistence variables from the database for a
     * given $id into a Pool object, and return the object.
     *
     * @param int $id Value of the $id for the Pool that will be
     *                instantiated.
     *
     * @return Pool $pool Pool Instance.
     */
    public function getInstanceFromId(int $id) : Pool
    {
        $specimenDAO = new SpecimenDAO($this->db);
        $specimenIds = $this->getSpecimenIdsFromId($id);
        $specimen    = $specimenDAO->getInstanceFromId(intval($specimenIds[0]));

        $poolArray                = $this->getInstanceDataFromId($id);
        $poolArray['CandidateID'] = $specimen->getCandidateId();
        $poolArray['SessionID']   = $specimen->getSessionId();
        $poolArray['TypeID']      = $specimen->getTypeId();

        $pool = $this->getInstanceFromSQL($poolArray, $specimenIds);

        return $pool;
    }

    /**
     * Queries data from the databse for the Pool at the requested ID.
     *
     * @param int $id Value of the $id for the Pool whose data is being queried.
     *
     * @return array Pool Data at that ID.
     */
    private function getInstanceDataFromId(int $id)
    {
        // TODO: query date and time format from backend to know how to query
        // them from the database.
        $query = "SELECT bp.PoolID,
                         bp.Label,
                         bp.Quantity,
                         bp.UnitID,
                         bp.CenterID,
                         bp.Date,
                         DATE_FORMAT(bp.Time, '%H:%i') as Time
                  FROM   biobank_pool bp
                  WHERE  bp.PoolID=:i";
        return $this->db->pselectRow($query, array('i' => $id));
    }

    /**
     * This function returns an array of all the Specimen Ids that are
     * associated with the given Pool Instance's ID in the
     * biobank_specimen_pool_rel table.
     *
     * @param int $id Pool id from which to query Specimen IDs
     *
     * @return array $specimenIds array of associated Specimen IDs
     */
    private function getSpecimenIdsFromId(int $id) : array
    {
        $query = 'SELECT SpecimenID
                        FROM biobank_specimen_pool_rel
                        WHERE PoolID=:i';
        return $this->db->pselectcol($query, array('i' => $id));
    }

    // XXX: Currently this function is never used with any conditions passed as
    // paramters. Decide if this is a necessary feature.
    /**
     * This will select all pools from the database that match the
     * attribute values passed through $poolData and will return an array
     * of pool objects.
     *
     * @param array $conditions Conditions to be met by the query
     *
     * @return array $pools      List of Pool Objects that match the query
     */
    public function selectInstances(array $conditions = null) : array
    {
        $query  = 'SELECT PoolID
                  FROM biobank_pool';
        $query .= $conditions ? ' WHERE '.$this->db->implodeWithKeys(' AND ', $conditions) : '';

        $result = $this->db->pselect($query, array());
        $pools  = array();
        if (!empty($result)) {
            foreach ($result as $row) {
                $id         = (int) $row['PoolID'];
                $pool       = $this->getInstanceFromId($id);
                $pools[$id] = $pool;
            }
        }

        return $pools;
    }

    /**
     * Instantiates an ArrayIterator class that is composed of all the Pool
     * Objects
     *
     * @return traversable Iterator of Pool Objects
     */
    protected function getAllInstances() : \Traversable
    {
        return new \ArrayIterator($this->selectInstances());
    }

    /**
     * This function recieves a Pool Object, converts it into an SQL format
     * and inserts it into all relevant tables in the database. Certain
     * information is then cascaded to the children of the Pool.
     *
     * @param Pool $pool The Pool to be inserted into the database.
     *
     * @return void
     */
    public function saveInstance(Pool $pool) : void
    {
        //Convert Pool to SQL format.
        $data = $this->getSQLFromInstance($pool);

        // TODO: Introduce a SQL Transaction here with a try catch.
        // Insert or Update the biobank_pool table with respective data.
        $this->db->insertOnDuplicateUpdate(
            'biobank_pool',
            $data['biobank_pool']
        );

        $poolId = $pool->getId();
        if (isset($poolId)) {
            // If update:
            // Do nothing.
        } else {
            // If insert:
            $poolId = $this->db->getLastInsertId();
            // set the respective data array index to the value of the new
            // pool ID
            $specimenIds = $pool->getSpecimenIds();
            foreach ($specimenIds as $id) {
                $data['biobank_specimen_pool_rel'][$id]['PoolID'] = $poolId;
            }
        }

        // insert or update biobank_specimen_pool_rel with relevant data.
        foreach ($data['biobank_specimen_pool_rel'] as $insert) {
            $this->db->insertOnDuplicateUpdate(
                'biobank_specimen_pool_rel',
                $insert
            );
        }

        // XXX: decide if quantity of all pooled specimens should go to 0.
        // XXX: logic for adding preparations to pools will likely go here as well.
    }

    /**
     * This function takes a Pool array and prepares the data to be inserted
     * into the database by converting it to a data array. This mapping is done
     * to enable the update or insertion of data into the database directly from
     * the resuling array.
     *
     * @param Pool $pool Pool array to be converted
     *
     * @return array $data Array containing the data to be inserted
     */
    private function getSQLFromInstance(Pool $pool) : array
    {
        $poolData = array(
                     'PoolID'   => $pool->getId(),
                     'Label'    => $pool->getLabel(),
                     'Quantity' => $pool->getQuantity(),
                     'UnitID'   => $pool->getUnitId(),
                     'CenterID' => $pool->getCenterId(),
                     'Date'     => $pool->getDate()->format('Y-m-d'),
                     'Time'     => $pool->getTime()->format('H:i'),
                    );

        $specimenIds = $pool->getSpecimenIds();
        foreach ($specimenIds as $id) {
            $specimenPoolData[$id] = array(
                                      'PoolID'     => $pool->getId(),
                                      'SpecimenID' => $id,
                                     );
        }

        return array(
                'biobank_pool'              => $poolData,
                'biobank_specimen_pool_rel' => $specimenPoolData,
               );
    }

    /**
     * This function takes an array that resulted from an SQL query and
     * instantiates it as a Pool Object
     *
     * @param array $data        Values to be reassigned.
     * @param array $specimenIds List of specimen IDs associated with the given
     *                           Pool.
     *
     * @return Pool
     */
    private function getInstanceFromSQL(array $data, array $specimenIds) : Pool
    {
        $pool = new Pool();
        isset($data['PoolID'])      && $pool->setId((int) $data['PoolID']);
        isset($data['Label'])       && $pool->setLabel((string) $data['Label']);
        isset($data['Quantity'])    && $pool->setQuantity((float) $data['Quantity']);
        isset($data['UnitID'])      && $pool->setUnitId((int) $data['UnitID']);
        !empty($specimenIds)        && $pool->setSpecimenIds($specimenIds);
        isset($data['CandidateID']) && $pool->setCandidateId((int) $data['CandidateID']);
        isset($data['SessionID'])   && $pool->setSessionId((int) $data['SessionID']);
        isset($data['TypeID'])      && $pool->setTypeId((int) $data['TypeID']);
        isset($data['CenterID'])    && $pool->setCenterId((int) $data['CenterID']);
        isset($data['Date'])        && $pool->setDate(new \DateTime($data['Date']));
        isset($data['Time'])        && $pool->setTime(new \DateTime($data['Time']));

        return $pool;
    }
}